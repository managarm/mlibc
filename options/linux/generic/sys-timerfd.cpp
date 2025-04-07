
#include <errno.h>
#include <sys/timerfd.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

int timerfd_create(int clockid, int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_timerfd_create, -1);
	int fd;
	if(int e = mlibc::sys_timerfd_create(clockid, flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int timerfd_settime(int fd, int flags, const struct itimerspec *value,
		struct itimerspec *oldvalue) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_timerfd_settime, -1);
	if(int e = mlibc::sys_timerfd_settime(fd, flags, value, oldvalue); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int timerfd_gettime(int fd, struct itimerspec *its) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_timerfd_gettime, -1);
	if(int e = sysdep(fd, its); e) {
		errno = e;
		return -1;
	}
	return 0;
}

