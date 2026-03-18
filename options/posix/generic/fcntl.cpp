
#include <errno.h>
#include <bits/ensure.h>
#include <fcntl.h>
#include <stdarg.h>

#include <mlibc-config.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int creat(const char *pathname, mode_t mode) {
	return open(pathname, O_CREAT|O_WRONLY|O_TRUNC, mode);
}

int fcntl(int fd, int command, ...) {
	va_list args;
	va_start(args, command);
	int result;
	if(int e = mlibc::sysdep_or_enosys<Fcntl>(fd, command, args, &result); e) {
		va_end(args);
		errno = e;
		return -1;
	}
	va_end(args);
	return result;
}

int openat(int dirfd, const char *pathname, int flags, ...) {
	va_list args;
	va_start(args, flags);
	mode_t mode = 0;
	int fd;

	if((flags & O_CREAT || (flags & O_TMPFILE) == O_TMPFILE))
		mode = va_arg(args, mode_t);

	if(int e = mlibc::sysdep_or_enosys<Openat>(dirfd, pathname, flags, mode, &fd); e) {
		va_end(args);
		errno = e;
		return -1;
	}
	va_end(args);
	return fd;
}

int posix_fadvise(int fd, off_t offset, off_t length, int advice) {
	// posix_fadvise() returns an error instead of setting errno.
	return mlibc::sysdep_or_enosys<Fadvise>(fd, offset, length, advice);
}

int posix_fallocate(int fd, off_t offset, off_t size) {
	// posix_fallocate() returns an error instead of setting errno.
	return mlibc::sysdep_or_enosys<Fallocate>(fd, offset, size);
}

int open(const char *pathname, int flags, ...) {
	mode_t mode = 0;

	if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE) {
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, mode_t);
		va_end(args);
	}

	int fd;
	if(int e = mlibc::sysdep<Open>(pathname, flags, mode, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("open")]] int open64(const char *pathname, int flags, ...);
#endif /* !__MLIBC_LINUX_OPTION */
