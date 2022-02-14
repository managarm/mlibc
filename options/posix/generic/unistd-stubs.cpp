#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <termios.h>
#include <sys/ioctl.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/arch-defs.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/thread.hpp>

unsigned int alarm(unsigned int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int chdir(const char *path) {
	if(!mlibc::sys_chdir) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_chdir(path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchdir(int fd) {
	if(!mlibc::sys_fchdir) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_fchdir(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int chown(const char *path, uid_t uid, gid_t gid) {
	if(!mlibc::sys_fchownat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_fchownat(AT_FDCWD, path, uid, gid, 0); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t confstr(int name, char *buf, size_t len) {
	const char *str = "";
	if (name == _CS_PATH) {
		str = "/bin:/usr/bin";
	} else if(name == _CS_GNU_LIBPTHREAD_VERSION) {
		// We are not glibc, so we can return 0 here.
		return 0;
	} else if(name == _CS_GNU_LIBC_VERSION) {
		// We are not glibc, so we can return 0 here.
		return 0;
	} else {
		mlibc::infoLogger() << "\e[31mmlibc: confstr() request " << name << " is unimplemented\e[39m"
				<< frg::endlog;
		__ensure(!"Not implemented");
	}

	return snprintf(buf, len, "%s", str) + 1;
}

void _exit(int status) {
	mlibc::sys_exit(status);
}

int execl(const char *path, const char *arg0, ...) {
	// TODO: It's a stupid idea to limit the number of arguments here.
	char *argv[16];
	argv[0] = const_cast<char *>(arg0);

	va_list args;
	int n = 1;
	va_start(args, arg0);
	while(true) {
		__ensure(n < 15);
		auto argn = va_arg(args, const char *);
		argv[n++] = const_cast<char *>(argn);
		if(!argn)
			break;
	}
	va_end(args);
	argv[n] = nullptr;

	return execve(path, argv, environ);
}

// This function is taken from musl.
int execle(const char *path, const char *arg0, ...) {
	int argc;
	va_list ap;
	va_start(ap, arg0);
	for(argc = 1; va_arg(ap, const char *); argc++);
	va_end(ap);

	int i;
	char *argv[argc + 1];
	char **envp;
	va_start(ap, arg0);
	argv[0] = (char *)argv;
	for(i = 1; i <= argc; i++)
		argv[i] = va_arg(ap, char *);
	envp = va_arg(ap, char **);
	va_end(ap);
	return execve(path, argv, envp);
}

int execlp(const char *, const char *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int execv(const char *path, char *const argv[]) {
	return execve(path, argv, environ);
}

int execvp(const char *file, char *const argv[]) {
	return execvpe(file, argv, environ);
}

int execvpe(const char *file, char *const argv[], char *const envp[]) {
	char *null_list[] = {
		nullptr
	};

	if(!argv)
		argv = null_list;
	if(!envp)
		envp = null_list;

	if(!mlibc::sys_execve) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	if(strchr(file, '/')) {
		int e = mlibc::sys_execve(file, argv, envp);
		__ensure(e && "sys_execve() is supposed to never return with success");
		errno = e;
		return -1;
	}

	frg::string_view dirs;
	if(const char *pv = getenv("PATH"); pv) {
		dirs = pv;
	}else{
		dirs = "/bin:/usr/bin";
	}

	size_t p = 0;
	int res = ENOENT;
	while(p < dirs.size()) {
		size_t s; // Offset of next colon or end of string.
		if(size_t cs = dirs.find_first(':', p); cs != size_t(-1)) {
			s = cs;
		}else{
			s = dirs.size();
		}

		frg::string<MemoryAllocator> path{getAllocator()};
		path += dirs.sub_string(p, s - p);
		path += "/";
		path += file;

		mlibc::infoLogger() << "mlibc: execvpe() tries '" << path.data() << "'" << frg::endlog;
		int e = mlibc::sys_execve(path.data(), argv, envp);
		__ensure(e && "sys_execve() is supposed to never return with success");
		switch(e) {
		case ENOENT:
		case ENOTDIR:
			break;
		case EACCES:
			res = EACCES;
			break;
		default:
			errno = e;
			return -1;
		}

		p = s + 1;
	}

	errno = res;
	return -1;
}

int faccessat(int dirfd, const char *pathname, int mode, int flags) {
	if(!mlibc::sys_faccessat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_faccessat(dirfd, pathname, mode, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchown(int fd, uid_t uid, gid_t gid) {
	if(!mlibc::sys_fchownat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_fchownat(fd, "", uid, gid, AT_EMPTY_PATH); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchownat(int fd, const char *path, uid_t uid, gid_t gid, int flags) {
	if(!mlibc::sys_fchownat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_fchownat(fd, path, uid, gid, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fdatasync(int fd) {
	if(!mlibc::sys_fdatasync) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_fdatasync(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fexecve(int, char *const [], char *const []) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long fpathconf(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fsync(int fd) {
	if(!mlibc::sys_fsync) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(auto e = mlibc::sys_fsync(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int ftruncate(int fd, off_t size) {
	if(!mlibc::sys_ftruncate) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_ftruncate(fd, size); e) {
		errno = e;
		return -1;
	}
	return 0;
}

char *getcwd(char *buffer, size_t size) {
	/* In order to support glibc's extension of allocating buffer if none is given, we have this
	   buffer to use as needed. We do not respect the size passed, which glibc does, but musl
	   doesn't do. This should be fine, as this behavior is an extension anyways and thus not part
	   of the spec. */
	char alt_buffer[PATH_MAX];

	if(!buffer) {
		buffer = alt_buffer;
		size = PATH_MAX;
	} else if(!size) {
		errno = EINVAL;
		return NULL;
	}

	if(!mlibc::sys_getcwd) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return NULL;
	}
	if(int e = mlibc::sys_getcwd(buffer, size); e) {
		errno = e;
		return NULL;
	}
	return (buffer == alt_buffer) ? strdup(buffer) : buffer;
}

int getgroups(int size, gid_t list[]) {
	if(!mlibc::sys_getgroups) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	int ret;
	if(int e = mlibc::sys_getgroups(size, list, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}

long gethostid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int gethostname(char *buffer, size_t bufsize) {
	if(!mlibc::sys_gethostname) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(auto e = mlibc::sys_gethostname(buffer, bufsize); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sethostname(const char *buffer, size_t bufsize) {
	if(!mlibc::sys_sethostname) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(auto e = mlibc::sys_sethostname(buffer, bufsize); e) {
		errno = e;
		return -1;
	}
	return 0;
}

// Code taken from musl
char *getlogin(void) {
	return getenv("LOGNAME");
}

int getlogin_r(char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// optarg and optind are provided to us by the GLIBC part of the mlibc.

static char *scan = NULL; /* Private scan pointer. */

int getopt(int argc, char *const argv[], const char *optstring) {
	char c;
	char *place;

	optarg = NULL;

	if (!scan || *scan == '\0') {
		if (optind == 0)
			optind++;

		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
			return EOF;
		if (argv[optind][1] == '-' && argv[optind][2] == '\0') {
			optind++;
			return EOF;
		}

		scan = argv[optind]+1;
		optind++;
	}

	c = *scan++;
	place = strchr(optstring, c);

	if (!place || c == ':') {
		fprintf(stderr, "%s: unknown option -%c\n", argv[0], c);
		return '?';
	}

	place++;
	if (*place == ':') {
		if (*scan != '\0') {
			optarg = scan;
			scan = NULL;
		} else if( optind < argc ) {
			optarg = argv[optind];
			optind++;
		} else {
			fprintf(stderr, "%s: option requires argument -%c\n", argv[0], c);
			return ':';
		}
	}

	return c;
}

pid_t getpgid(pid_t pid) {
	pid_t pgid;

	if(!mlibc::sys_getpgid) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_getpgid(pid, &pgid); e) {
		errno = e;
		return -1;
	}
	return pgid;
}

pid_t getpgrp(void) {
	return getpgid(0);
}

pid_t getsid(pid_t pid) {
	if(!mlibc::sys_getsid) {
		MLIBC_MISSING_SYSDEP();
		return -1;
	}
	pid_t sid;
	if(int e = mlibc::sys_getsid(pid, &sid); e) {
		errno = e;
		return -1;
	}
	return sid;
}

int lchown(const char *path, uid_t uid, gid_t gid) {
	if(!mlibc::sys_fchownat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_fchownat(AT_FDCWD, path, uid, gid, AT_SYMLINK_NOFOLLOW); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int link(const char *old_path, const char *new_path) {
	if(!mlibc::sys_link) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_link(old_path, new_path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	if(!mlibc::sys_linkat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_linkat(olddirfd, old_path, newdirfd, new_path, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

// Code take from musl
int lockf(int fd, int op, off_t size) {
	struct flock l = {
		.l_type = F_WRLCK,
		.l_whence = SEEK_CUR,
		.l_start = 0,
		.l_len = size,
		.l_pid = 0,
	};

	switch(op) {
		case F_TEST:
			l.l_type = F_RDLCK;
			if(fcntl(fd, F_GETLK, &l) < 0)
				return -1;
			if(l.l_type == F_UNLCK || l.l_pid == getpid())
				return 0;
			errno = EACCES;
			return -1;
		case F_ULOCK:
			l.l_type = F_UNLCK;
			[[fallthrough]];
		case F_TLOCK:
			return fcntl(fd, F_SETLK, &l);
		case F_LOCK:
			return fcntl(fd, F_SETLKW, &l);
	}

	errno = EINVAL;
	return -1;
}

int nice(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

long pathconf(const char *, int name) {
	switch (name) {
	case _PC_NAME_MAX:
		return NAME_MAX;
	default:
		mlibc::infoLogger() << "missing pathconf() entry " << name << frg::endlog;
		errno = EINVAL;
		return -1;
	}
}

int pause(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pipe(int *fds) {
	if(!mlibc::sys_pipe) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_pipe(fds, 0); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int pipe2(int *fds, int flags) {
	if(!mlibc::sys_pipe) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_pipe(fds, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t pread(int fd, void *buf, size_t n, off_t off) {
	ssize_t num_read;

	if(!mlibc::sys_pread) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_pread(fd, buf, n, off, &num_read); e) {
		errno = e;
		return -1;
	}
	return num_read;
}

ssize_t pwrite(int, const void *, size_t, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t readlink(const char *__restrict path, char *__restrict buffer, size_t max_size) {
	ssize_t length;
	if(!mlibc::sys_readlink) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_readlink(path, buffer, max_size, &length); e) {
		errno = e;
		return -1;
	}
	return length;
}

ssize_t readlinkat(int, const char *__restrict, char *__restrict, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int rmdir(const char *path) {
	if(!mlibc::sys_rmdir) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_rmdir(path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setegid(gid_t egid) {
	if(!mlibc::sys_setegid) {
		MLIBC_MISSING_SYSDEP();
		mlibc::infoLogger() << "mlibc: missing sysdep sys_setegid(). Returning 0" << frg::endlog;
		errno = ENOSYS;
		return 0;
	}
	if(int e = mlibc::sys_setegid(egid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int seteuid(uid_t euid) {
	if(!mlibc::sys_seteuid) {
		MLIBC_MISSING_SYSDEP();
		mlibc::infoLogger() << "mlibc: missing sysdep sys_seteuid(). Returning 0" << frg::endlog;
		errno = ENOSYS;
		return 0;
	}
	if(int e = mlibc::sys_seteuid(euid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setgid(gid_t gid) {
	if(!mlibc::sys_setgid) {
		MLIBC_MISSING_SYSDEP();
		mlibc::infoLogger() << "mlibc: missing sysdep sys_setgid(). Returning 0" << frg::endlog;
		errno = ENOSYS;
		return 0;
	}
	if(int e = mlibc::sys_setgid(gid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setpgid(pid_t pid, pid_t pgid) {
	if(!mlibc::sys_setpgid) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_setpgid(pid, pgid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

pid_t setpgrp(pid_t pid, pid_t pgid) {
	return setpgid(pid, pgid);
}

int setregid(gid_t, gid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setreuid(uid_t, uid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

pid_t setsid(void) {
	if(!mlibc::sys_setsid) {
		MLIBC_MISSING_SYSDEP();
		return -1;
	}
	pid_t sid;
	if(int e = mlibc::sys_setsid(&sid); e) {
		errno = e;
		return -1;
	}
	return sid;
}

int setuid(uid_t uid) {
	if(!mlibc::sys_setuid) {
		MLIBC_MISSING_SYSDEP();
		mlibc::infoLogger() << "mlibc: missing sysdep sys_setuid(). Returning 0" << frg::endlog;
		return 0;
	}
	if(int e = mlibc::sys_setuid(uid); e) {
		errno = e;
		return -1;
	}
	return 0;
}

void swab(const void *__restrict, void *__restrict, ssize_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int symlink(const char *target_path, const char *link_path) {
	if(!mlibc::sys_symlink) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_symlink(target_path, link_path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int symlinkat(const char *target_path, int dirfd, const char *link_path) {
	if(!mlibc::sys_symlinkat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_symlinkat(target_path, dirfd, link_path); e) {
		errno = e;
		return -1;
	}
	return 0;
}

void sync(void) {
	if(!mlibc::sys_sync) {
		MLIBC_MISSING_SYSDEP();
	} else {
		mlibc::sys_sync();
	}
}

unsigned long sysconf(int number) {
	switch(number) {
		case _SC_ARG_MAX:
			// On linux, it is defined to 2097152 in most cases, so define it to be 2097152
			return 2097152;
		case _SC_PAGE_SIZE:
			return mlibc::page_size;
		case _SC_OPEN_MAX:
			// TODO: actually return a proper value for _SC_OPEN_MAX
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_OPEN_MAX) returns arbitrary value 256\e[39m" << frg::endlog;
			return 256;
		case _SC_PHYS_PAGES:
			// TODO: actually return a proper value for _SC_PHYS_PAGES
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_PHYS_PAGES) returns arbitrary value 1024\e[39m" << frg::endlog;
			return 1024;
		case _SC_NPROCESSORS_ONLN:
			// TODO: actually return a proper value for _SC_NPROCESSORS_ONLN
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_NPROCESSORS_ONLN) unconditionally returns 1\e[39m" << frg::endlog;
			return 1;
		case _SC_GETPW_R_SIZE_MAX:
			return 1024;
		case _SC_GETGR_R_SIZE_MAX:
			return 1024;
		case _SC_CHILD_MAX:
			// On linux, it is defined to 25 in most cases, so define it to be 25
			return 25;
		case _SC_JOB_CONTROL:
			// If 1, job control is supported
			return 1;
		case _SC_CLK_TCK:
			// TODO: This should be obsolete?
			mlibc::infoLogger() << "\e[31mmlibc: sysconf(_SC_CLK_TCK) is obsolete and returns arbitrary value 1000000\e[39m" << frg::endlog;
			return 1000000;
		case _SC_NGROUPS_MAX:
			// On linux, it is defined to 65536 in most cases, so define it to be 65536
			return 65536;
		case _SC_LINE_MAX:
			// Linux defines it as 2048.
			return 2048;
		case _SC_XOPEN_CRYPT:
#ifdef __MLIBC_CRYPT_OPTION
			return _XOPEN_CRYPT;
#else
			return -1;
#endif
		default:
			mlibc::panicLogger() << "\e[31mmlibc: sysconf() call is not implemented, number: " << number << "\e[39m" << frg::endlog;
			__builtin_unreachable();
	}
}

pid_t tcgetpgrp(int fd) {
	int pgrp;
	if(ioctl(fd, TIOCGPGRP, &pgrp) < 0) {
		return -1;
	}
	return pgrp;
}

int tcsetpgrp(int fd, pid_t pgrp) {
	return ioctl(fd, TIOCSPGRP, pgrp);
}

int truncate(const char *, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *ttyname(int fd) {
	const size_t size = 128;
	static thread_local char buf[size];
	if(!mlibc::sys_ttyname) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return nullptr;
	}
	if(int e = mlibc::sys_ttyname(fd, buf, size); e) {
		errno = e;
		return nullptr;
	}
	return buf;
}

int ttyname_r(int, char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int unlink(const char *path) {
	if(!mlibc::sys_unlinkat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_unlinkat(AT_FDCWD, path, 0); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int unlinkat(int fd, const char *path, int flags) {
	if(!mlibc::sys_unlinkat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_unlinkat(fd, path, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getpagesize() {
	return mlibc::page_size;
}

// Code taken from musl
// GLIBC extension for stdin/stdout
char *getpass(const char *prompt) {
	int fdin, fdout;
	struct termios s, t;
	ssize_t l;
	static char password[128];

	if((fdin = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC)) < 0) {
		fdin = STDIN_FILENO;
		fdout = STDOUT_FILENO;
	} else {
		fdout = fdin;
	}

	tcgetattr(fdin, &t);
	s = t;
	t.c_lflag &= ~(ECHO | ISIG);
	t.c_lflag |= ICANON;
	t.c_iflag &= ~(INLCR | IGNCR);
	t.c_iflag |= ICRNL;
	tcsetattr(fdin, TCSAFLUSH, &t);
	tcdrain(fdin);

	dprintf(fdout, "%s", prompt);

	l = read(fdin, password, sizeof password);
	if(l >= 0) {
		if((l > 0 && password[l - 1] == '\n') || l == sizeof password)
			l--;
		password[l] = 0;
	}

	tcsetattr(fdin, TCSAFLUSH, &s);

	dprintf(fdout, "\n");
	if(fdin != STDIN_FILENO) {
		close(fdin);
	}

	return l < 0 ? 0 : password;
}

char *get_current_dir_name(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// This is a Linux extension
pid_t gettid(void) {
	mlibc::infoLogger() << "\e[31mmlibc: gettid() is not implemented correctly\e[39m"
			<< frg::endlog;
	return mlibc::sys_getpid();
}

int getentropy(void *buffer, size_t length) {
	if(!mlibc::sys_getentropy) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(length > 256) {
		errno = EIO;
		return -1;
	}
	if(int e = mlibc::sys_getentropy(buffer, length); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t write(int fd, const void *buf, size_t count) {
	ssize_t bytes_written;
	if(int e = mlibc::sys_write(fd, buf, count, &bytes_written); e) {
		errno = e;
		return (ssize_t)-1;
	}
	return bytes_written;
}

ssize_t read(int fd, void *buf, size_t count) {
	ssize_t bytes_read;
	if(int e = mlibc::sys_read(fd, buf, count, &bytes_read); e) {
		errno = e;
		return (ssize_t)-1;
	}
	return bytes_read;
}

off_t lseek(int fd, off_t offset, int whence) {
	off_t new_offset;
	if(int e = mlibc::sys_seek(fd, offset, whence, &new_offset); e) {
		errno = e;
		return (off_t)-1;
	}
	return new_offset;
}

off64_t lseek64(int fd, off64_t offset, int whence) {
	off64_t new_offset;
	if(int e = mlibc::sys_seek(fd, offset, whence, &new_offset); e) {
		errno = e;
		return (off64_t)-1;
	}
	return new_offset;
}

int close(int fd) {
	return mlibc::sys_close(fd);
}

unsigned int sleep(unsigned int secs) {
	time_t seconds = secs;
	long nanos = 0;
	if(!mlibc::sys_sleep) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_sleep()");
	}
	mlibc::sys_sleep(&seconds, &nanos);
	return seconds;
}

// In contrast to sleep() this functions returns 0/-1 on success/failure.
int usleep(useconds_t usecs) {
	time_t seconds = 0;
	long nanos = usecs * 1000;
	if(!mlibc::sys_sleep) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_sleep()");
	}
	return mlibc::sys_sleep(&seconds, &nanos);
}

int dup(int fd) {
	int newfd;
	if(!mlibc::sys_dup) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_dup(fd, 0, &newfd); e) {
		errno = e;
		return -1;
	}
	return newfd;
}

int dup2(int fd, int newfd) {
	if(!mlibc::sys_dup2) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_dup2(fd, 0, newfd); e) {
		errno = e;
		return -1;
	}
	return newfd;
}

pid_t fork(void) {
	auto self = mlibc::get_current_tcb();
	pid_t child;

	if(!mlibc::sys_fork) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	auto hand = self->atforkEnd;
	while (hand) {
		if (hand->prepare)
			hand->prepare();

		hand = hand->prev;
	}

	if(int e = mlibc::sys_fork(&child); e) {
		errno = e;
		return -1;
	}

	hand = self->atforkBegin;
	while (hand) {
		if (!child) {
			if (hand->child)
				hand->child();
		} else {
			if (hand->parent)
				hand->parent();
		}
		hand = hand->next;
	}

	return child;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	char *null_list[] = {
		nullptr
	};

	if(!argv)
		argv = null_list;
	if(!envp)
		envp = null_list;

	if(!mlibc::sys_execve) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	int e = mlibc::sys_execve(path, argv, envp);
	__ensure(e && "sys_execve() is expected to fail if it returns");
	errno = e;
	return -1;
}

gid_t getgid(void) {
	if(!mlibc::sys_getgid) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getgid()");
	}
	return mlibc::sys_getgid();
}

gid_t getegid(void) {
	if(!mlibc::sys_getegid) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getegid()");
	}
	return mlibc::sys_getegid();
}

uid_t getuid(void) {
	if(!mlibc::sys_getuid) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getuid()");
	}
	return mlibc::sys_getuid();
}

uid_t geteuid(void) {
	if(!mlibc::sys_geteuid) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_geteuid()");
	}
	return mlibc::sys_geteuid();
}

pid_t getpid(void) {
	if(!mlibc::sys_getpid) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getpid()");
	}
	return mlibc::sys_getpid();
}

pid_t getppid(void) {
	if(!mlibc::sys_getppid) {
		MLIBC_MISSING_SYSDEP();
		__ensure(!"Cannot continue without sys_getppid()");
	}
	return mlibc::sys_getppid();
}

int access(const char *path, int mode) {
	if(!mlibc::sys_access) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_access(path, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

char *getusershell(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void setusershell(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void endusershell(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int isatty(int fd) {
	if(!mlibc::sys_isatty) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return 0; // isatty() returns 0 on failure.
	}
	if(int e = mlibc::sys_isatty(fd); e) {
		errno = e;
		return 0;
	}
	return 1;
}

int chroot(const char *ptr) {
	if(!mlibc::sys_chroot) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_chroot(ptr); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int daemon(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *ctermid(char *s) {
	return s ? strcpy(s, "/dev/tty") : const_cast<char *>("/dev/tty");
}

#ifdef __MLIBC_CRYPT_OPTION
void encrypt(char[64], int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
#endif
