#include <shadow.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

/*
 * The code in this file is largely based on or taken from musl.
 * This includes:
 * - xatol
 * - __parsespent
 * - cleanup
 * - getspnam_r
 * - getspnam
 */
#define NUM(n) ((n) == -1 ? 0 : -1), ((n) == -1 ? 0 : (n))

int putspent(const struct spwd *sp, FILE *f) {
	auto str = [] (char *s) {
		return ((s) ? (s) : "");
	};
	return fprintf(f, "%s:%s:%.*ld:%.*ld:%.*ld:%.*ld:%.*ld:%.*ld:%.*u\n",
		str(sp->sp_namp), str(sp->sp_pwdp), NUM(sp->sp_lstchg),
		NUM(sp->sp_min), NUM(sp->sp_max), NUM(sp->sp_warn),
		NUM(sp->sp_inact), NUM(sp->sp_expire), NUM((int)sp->sp_flag)) < 0 ? -1 : 0;
}
#undef NUM

static long xatol(char **s) {
	long x;
	if(**s == ':' || **s == '\n') {
		return -1;
	}
	for(x = 0; (unsigned int)**s - '0' < 10U; ++*s) {
		x = 10 * x + (**s - '0');
	}
	return x;
}

static int __parsespent(char *s, struct spwd *sp) {
	sp->sp_namp = s;
	if(!(s = strchr(s, ':'))) {
		return -1;
	}
	*s = 0;

	sp->sp_pwdp = ++s;
	if(!(s = strchr(s, ':'))) {
		return -1;
	}
	*s = 0;

	s++;
	sp->sp_lstchg = xatol(&s);
	if(*s != ':') {
		return -1;
	}

	s++;
	sp->sp_min = xatol(&s);
	if(*s != ':') {
		return -1;
	}

	s++;
	sp->sp_max = xatol(&s);
	if(*s != ':') {
		return -1;
	}

	s++;
	sp->sp_warn = xatol(&s);
	if(*s != ':') {
		return -1;
	}

	s++;
	sp->sp_inact = xatol(&s);
	if(*s != ':') {
		return -1;
	}

	s++;
	sp->sp_expire = xatol(&s);
	if(*s != ':') {
		return -1;
	}

	s++;
	sp->sp_flag = xatol(&s);
	if(*s != '\n') {
		return -1;
	}
	return 0;
}

static void cleanup(void *p) {
	fclose((FILE *)p);
}

int getspnam_r(const char *name, struct spwd *sp, char *buf, size_t size, struct spwd **res) {
	char path[20 + NAME_MAX];
	FILE *f = 0;
	int rv = 0;
	int fd;
	size_t k, l = strlen(name);
	int skip = 0;
	int cs;
	int orig_errno = errno;

	*res = 0;

	/* Disallow potentially-malicious user names */
	if(*name=='.' || strchr(name, '/') || !l) {
		return errno = EINVAL;
	}

	/* Buffer size must at least be able to hold name, plus some.. */
	if(size < l + 100) {
		return errno = ERANGE;
	}

	/* Protect against truncation */
	if(snprintf(path, sizeof path, "/etc/tcb/%s/shadow", name) >= (int)sizeof path) {
		return errno = EINVAL;
	}

	fd = open(path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
	if(fd >= 0) {
		struct stat st = {};
		errno = EINVAL;
		if(fstat(fd, &st) || !S_ISREG(st.st_mode) || !(f = fdopen(fd, "rb"))) {
			pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
			close(fd);
			pthread_setcancelstate(cs, 0);
			return errno;
		}
	} else {
		if(errno != ENOENT && errno != ENOTDIR) {
			return errno;
		}
		f = fopen("/etc/shadow", "rbe");
		if(!f) {
			if(errno != ENOENT && errno != ENOTDIR) {
				return errno;
			}
			return 0;
		}
	}

	pthread_cleanup_push(cleanup, f);
	while(fgets(buf, size, f) && (k = strlen(buf)) > 0) {
		if(skip || strncmp(name, buf, l) || buf[l] != ':') {
			skip = buf[k - 1] != '\n';
			continue;
		}
		if(buf[k - 1] != '\n') {
			rv = ERANGE;
			break;
		}

		if(__parsespent(buf, sp) < 0) {
			continue;
		}
		*res = sp;
		break;
	}
	pthread_cleanup_pop(1);
	errno = rv ? rv : orig_errno;
	return rv;
}

int lckpwdf(void) {
	mlibc::infoLogger() << "mlibc: lckpwdf is unimplemented like musl" << frg::endlog;
	return 0;
}

int ulckpwdf(void) {
	mlibc::infoLogger() << "mlibc: ulckpwdf is unimplemented like musl" << frg::endlog;
	return 0;
}

// Musl defines LINE_LIM to 256
#define LINE_LIM 256

struct spwd *getspnam(const char *name) {
	static struct spwd sp;
	static char *line;
	struct spwd *res;
	int e;
	int orig_errno = errno;

	if(!line) {
		line = (char *)malloc(LINE_LIM);
	}
	if(!line) {
		return 0;
	}
	e = getspnam_r(name, &sp, line, LINE_LIM, &res);
	errno = e ? e : orig_errno;
	return res;
}

struct spwd *fgetspent(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void endspent(void) {
	mlibc::infoLogger() << "mlibc: endspent is a stub" << frg::endlog;
}

struct spwd *sgetspent(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
