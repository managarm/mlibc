
#include <errno.h>
#include <sys/ioctl.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/glibc-sysdeps.hpp>

int ioctl(int fd, unsigned long request, ...) {
	va_list args;
	va_start(args, request);
	int result;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_ioctl, -1);
	void *arg = va_arg(args, void *);
	if(int e = mlibc::sys_ioctl(fd, request, arg, &result); e) {
		errno = e;
		return -1;
	}
	return result;
}

