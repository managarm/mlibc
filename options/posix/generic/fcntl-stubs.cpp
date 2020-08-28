
#include <errno.h>
#include <bits/ensure.h>
#include <fcntl.h>
#include <stdarg.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int creat(const char *pathname, mode_t mode) {
	return open(pathname, O_CREAT|O_WRONLY|O_TRUNC, mode);
}

int fcntl(int fd, int command, ...) {
	va_list args;
	va_start(args, command);
	int result;
	if(!mlibc::sys_fcntl) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_fcntl(fd, command, args, &result); e) {
		errno = e;
		return -1;
	}
	va_end(args);
	return result;
}

int openat(int dirfd, const char *pathname, int flags, ...) {
	int fd;
	if(!mlibc::sys_openat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_openat(dirfd, pathname, flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int posix_fadvise(int fd, off_t offset, off_t length, int advice) {
	if(!mlibc::sys_fadvise) {
		mlibc::infoLogger() << "mlibc: fadvise() ignored due to missing sysdep" << frg::endlog;
		return 0;
	}

	// posix_fadvise() returns an error instead of setting errno.
	return mlibc::sys_fadvise(fd, offset, length, advice);
}

int posix_fallocate(int fd, off_t offset, off_t size) {
	// posix_fallocate() returns an error instead of setting errno.
	if(!mlibc::sys_fallocate) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}
	return mlibc::sys_fallocate(fd, offset, size);
}

// This is a linux extension
int name_to_handle_at(int, const char *, struct file_handle *, int *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int open_by_handle_at(int, struct file_handle *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int open(const char *pathname, int flags, ...) {
	int fd;
	if(int e = mlibc::sys_open(pathname, flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;

}

