
#include <errno.h>
#include <sys/signalfd.h>

#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>

int signalfd(int fd, const sigset_t *mask, int flags) {
	__ensure(fd == -1);
	if(!mlibc::sys_signalfd_create) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_signalfd_create(*mask, flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

