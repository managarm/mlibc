
#include <sys/ioctl.h>
#include <bits/ensure.h>
#include <frigg/debug.hpp>

#include <mlibc/sysdeps.hpp>

int ioctl(int fd, unsigned long request, void *arg) {
	return mlibc::sys_ioctl(fd, request, arg);
}

