
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

int settimeofday(const struct timeval *tv, const struct timezone *) {
	if(!tv)
		return 0;
	// tv_usec must be in the range 0, 999999
	if(tv->tv_usec >= 1000000) {
		errno = EINVAL;
		return -1;
	}
	if(int e = mlibc::sys_clock_set(CLOCK_REALTIME, tv->tv_sec, tv->tv_usec * 1000); e) {
		errno = e;
		return -1;
	}
	return 0;
}

void timeradd(const struct timeval *a, const struct timeval *b, struct timeval *res) {
	res->tv_sec = a->tv_sec + b->tv_sec;
	res->tv_usec = a->tv_usec + b->tv_usec;
	while(res->tv_usec > 999999) {
		res->tv_usec -= 1000000;
		res->tv_sec += 1;
	}
}

void timersub(const struct timeval *a, const struct timeval *b, struct timeval *res) {
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_usec = a->tv_usec - b->tv_usec;
	while(res->tv_usec < 0) {
		res->tv_usec += 1000000;
		res->tv_sec -= 1;
	}
}

void timerclear(struct timeval *tvp) {
	tvp->tv_sec = 0;
	tvp->tv_usec = 0;
}

int timerisset(struct timeval *tvp) {
	if(tvp->tv_sec != 0 || tvp->tv_usec != 0) {
		return 1;
	}
	return 0;
}

int getitimer(int which, struct itimerval *curr_value) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getitimer, -1);
	if(int e = mlibc::sys_getitimer(which, curr_value); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setitimer, -1);
	if(int e = mlibc::sys_setitimer(which, new_value, old_value); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int timer_create(clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_timer_create, -1);
	if(int e = mlibc::sys_timer_create(clk, evp, res); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int timer_settime(timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_timer_settime, -1);
	if(int e = mlibc::sys_timer_settime(t, flags, val, old); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int timer_gettime(timer_t, struct itimerspec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int timer_delete(timer_t t) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_timer_delete, -1);
	if(int e = mlibc::sys_timer_delete(t); e) {
		errno = e;
		return -1;
	}
	return 0;
}
