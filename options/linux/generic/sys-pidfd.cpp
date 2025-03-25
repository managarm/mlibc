#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>
#include <sys/pidfd.h>

int pidfd_open(pid_t pid, unsigned int flags) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_pidfd_open, -1);
	int fd = 0;

	if(int e = sysdep(pid, flags, &fd); e) {
		errno = e;
		return -1;
	}

	return fd;
}

pid_t pidfd_getpid(int fd) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_pidfd_getpid, -1);
	pid_t pid = 0;

	if(int e = sysdep(fd, &pid); e) {
		errno = e;
		return -1;
	}

	return pid;
}
