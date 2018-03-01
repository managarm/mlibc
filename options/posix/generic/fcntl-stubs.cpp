
#include <errno.h>
#include <bits/ensure.h>
#include <fcntl.h>
#include <stdarg.h>

#include <mlibc/sysdeps.hpp>

int creat(const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fcntl(int fd, int command, ...) {
	va_list args;
	va_start(args, command);
	int val = mlibc::sys_fcntl(fd, command, args);
	va_end(args);
	return val;
}

int openat(int, const char *, int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int posix_fadvise(int, off_t, off_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int posix_fallocate(int fd, off_t offset, off_t size) {
	struct error_guard {
		error_guard()
		: _s{errno} { }

		~error_guard() {
			errno = _s;
		}

	private:
		int _s;
	};

	error_guard guard;

	if(mlibc::sys_fallocate(fd, offset, size))
		return errno;
	return 0;
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
	if(mlibc::sys_open(pathname, flags, &fd))
		return -1;
	return fd;

}

