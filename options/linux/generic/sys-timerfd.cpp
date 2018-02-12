
#include <bits/ensure.h>
#include <sys/timerfd.h>

#include <mlibc/sysdeps.hpp>

int timerfd_create(int, int flags) {
	int fd;
	if(mlibc::sys_timerfd_create(flags, &fd))
		return -1;
	return fd;
}

int timerfd_settime(int, int, const struct itimerspec *, struct itimerspec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int timerfd_gettime(int, struct itimerspec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

