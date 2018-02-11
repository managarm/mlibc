
#include <bits/ensure.h>
#include <sys/signalfd.h>

#include <mlibc/sysdeps.hpp>

int signalfd(int fd, const sigset_t *, int flags) {
	__ensure(fd == -1);
	if(mlibc::sys_signalfd_create(flags, &fd))
		return -1;
	return fd;
}

