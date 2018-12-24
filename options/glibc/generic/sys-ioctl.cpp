
#include <errno.h>
#include <sys/ioctl.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

int ioctl(int fd, unsigned long request, void *arg) {
	int result;
	if(int e = mlibc::sys_ioctl(fd, request, arg, &result); e) {
		errno = e;
		return -1;
	}
	return result;
}

