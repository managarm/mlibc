
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int gettimeofday(struct timeval *__restrict result, void *__restrict unused) {
	(void)unused; // Linux just ignores gettimeofday().

	if(result) {
		long nanos;
		if(int e = mlibc::sys_clock_get(CLOCK_REALTIME, &result->tv_sec, &nanos); e) {
			errno = e;
			return -1;
		}
		result->tv_usec = nanos / 1000;
	}
	return 0;
}

void timeradd(struct timeval *, struct timeval *, struct timeval *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void timersub(struct timeval *, struct timeval *, struct timeval *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void timerclear(struct timeval *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int timerisset(struct timeval *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getitimer(int which, struct itimerval *curr_value) {
	if(!mlibc::sys_getitimer) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_getitimer(which, curr_value); e) {
		errno = e;
		return -1;
	}
}

int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
	if(!mlibc::sys_setitimer) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_setitimer(which, new_value, old_value); e) {
		errno = e;
		return -1;
	}
}
