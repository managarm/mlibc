
#include <bits/ensure.h>
#include <sys/timerfd.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

int timerfd_create(int, int flags) {
	int fd;
	if(mlibc::sys_timerfd_create(flags, &fd))
		return -1;
	return fd;
}

int timerfd_settime(int fd, int flags, const struct itimerspec *value,
		struct itimerspec *oldvalue) {
	__ensure(!oldvalue);

	if(mlibc::sys_timerfd_settime(fd, flags, value))
		return -1;
	return 0;
}

int timerfd_gettime(int, struct itimerspec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

