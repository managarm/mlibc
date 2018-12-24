
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

	if(int e = mlibc::sys_fallocate(fd, offset, size); e) {
		errno = e;
		return -1;
	}
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
	if(int e = mlibc::sys_open(pathname, flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;

}

