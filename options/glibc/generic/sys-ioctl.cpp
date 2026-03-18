
#include <errno.h>
#include <sys/ioctl.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int ioctl(int fd, unsigned long request, ...) {
	va_list args;
	va_start(args, request);
	int result;
	void *arg = va_arg(args, void *);
	if(int e = mlibc::sysdep_or_enosys<Ioctl>(fd, request, arg, &result); e) {
		va_end(args);
		errno = e;
		return -1;
	}
	va_end(args);
	return result;
}

