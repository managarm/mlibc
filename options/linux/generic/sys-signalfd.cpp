
#include <errno.h>
#include <sys/signalfd.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

int signalfd(int fd, const sigset_t *mask, int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_signalfd_create, -1);
	if(int e = mlibc::sys_signalfd_create(mask, flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

