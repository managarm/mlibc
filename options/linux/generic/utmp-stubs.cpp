#include <utmp.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

/*
 * The code in this file is largely based on glibc.
 * This includes:
 * - setutent
 * - read_last_entry
 * - getutent
 * - getutent_r
 * - endutent
 */
static int fd = -1;
static off_t offset;

static struct utmp last_entry;

void setutent(void) {
	if(fd < 0) {
		fd = open("/run/utmp", O_RDONLY | O_LARGEFILE | O_CLOEXEC);
		if(fd == -1) {
			return;
		}
	}

	lseek(fd, 0, SEEK_SET);
	offset = 0;
}

static ssize_t read_last_entry(void) {
	struct utmp buf;
	ssize_t bytes_read = pread(fd, &buf, sizeof(buf), offset);

	if(bytes_read < 0) {
		return -1;
	} else if(bytes_read != sizeof(buf)) {
		// EOF
		return 0;
	} else {
		last_entry = buf;
		offset += sizeof(buf);
		return 1;
	}
}

struct utmp *getutent(void) {
	struct utmp *result;
	static struct utmp *buf;
	if(buf == NULL) {
		buf = (struct utmp *)malloc(sizeof(struct utmp));
		if(buf == NULL) {
			return NULL;
		}
	}

	if(getutent_r(buf, &result) < 0) {
		return NULL;
	}
	return result;
}

int getutent_r(struct utmp *buf, struct utmp **res) {
	int saved_errno = errno;

	if(fd < 0) {
		setutent();
	}

	ssize_t bytes_read = read_last_entry();

	if(bytes_read <= 0) {
		if(bytes_read == 0) {
			errno = saved_errno;
			*res = NULL;
			return -1;
		}
	}

	memcpy(buf, &last_entry, sizeof(struct utmp));
	*res = buf;

	return 0;
}

void endutent(void) {
	if(fd >= 0) {
		close(fd);
		fd = -1;
	}
}

struct utmp *pututline(const struct utmp *) {
	mlibc::infoLogger() << "\e[31mmlibc: pututline() is a stub!\e[39m" << frg::endlog;
	return NULL;
}

struct utmp *getutline(const struct utmp *) {
	mlibc::infoLogger() << "\e[31mmlibc: getutline() is a stub!\e[39m" << frg::endlog;
	return NULL;
}

int utmpname(const char *) {
	mlibc::infoLogger() << "\e[31mmlibc: utmpname() is a stub!\e[39m" << frg::endlog;
	return -1;
}

struct utmp *getutid(const struct utmp *) {
	mlibc::infoLogger() << "\e[31mmlibc: getutid() is a stub!\e[39m" << frg::endlog;
	return NULL;
}
