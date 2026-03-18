
#include <errno.h>
#include <sys/timerfd.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int timerfd_create(int clockid, int flags) {
	int fd;
	if(int e = mlibc::sysdep_or_enosys<TimerfdCreate>(clockid, flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int timerfd_settime(int fd, int flags, const struct itimerspec *value,
		struct itimerspec *oldvalue) {
	if(int e = mlibc::sysdep_or_enosys<TimerfdSettime>(fd, flags, value, oldvalue); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int timerfd_gettime(int fd, struct itimerspec *its) {
	if(int e = mlibc::sysdep_or_enosys<TimerfdGettime>(fd, its); e) {
		errno = e;
		return -1;
	}
	return 0;
}

