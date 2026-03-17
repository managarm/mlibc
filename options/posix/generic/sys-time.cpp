
#include <errno.h>
#include <sys/time.h>
#include <time.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int gettimeofday(struct timeval *__restrict result, void *__restrict unused) {
	(void)unused; // Linux just ignores gettimeofday().

	if(result) {
		long nanos;
		if(int e = mlibc::sysdep<ClockGet>(CLOCK_REALTIME, &result->tv_sec, &nanos); e) {
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
	if(int e = mlibc::sysdep_or_enosys<ClockSet>(CLOCK_REALTIME, tv->tv_sec, tv->tv_usec * 1000); e) {
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
	if(int e = mlibc::sysdep_or_enosys<GetItimer>(which, curr_value); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
	if(int e = mlibc::sysdep_or_enosys<SetItimer>(which, new_value, old_value); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int timer_create(clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res) {
	if(int e = mlibc::sysdep_or_enosys<TimerCreate>(clk, evp, res); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int timer_settime(timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old) {
	if(int e = mlibc::sysdep_or_enosys<TimerSettime>(t, flags, val, old); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int timer_gettime(timer_t t, struct itimerspec *val) {
	if(int e = mlibc::sysdep_or_enosys<TimerGettime>(t, val); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int timer_delete(timer_t t) {
	if(int e = mlibc::sysdep_or_enosys<TimerDelete>(t); e) {
		errno = e;
		return -1;
	}
	return 0;
}
