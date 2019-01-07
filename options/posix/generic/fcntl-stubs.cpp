
#include <errno.h>
#include <bits/ensure.h>
#include <fcntl.h>
#include <stdarg.h>

#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

int creat(const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fcntl(int fd, int command, ...) {
	va_list args;
	va_start(args, command);
	int result;
	if(int e = mlibc::sys_fcntl(fd, command, args, &result); e) {
		errno = e;
		return -1;
	}
	va_end(args);
	return result;
}

int openat(int, const char *, int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

