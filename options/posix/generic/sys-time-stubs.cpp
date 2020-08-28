
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

void timeradd(struct timeval *a, struct timeval *b, struct timeval *res) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void timersub(struct timeval *a, struct timeval *b, struct timeval *res) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void timerclear(struct timeval *tvp) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int timerisset(struct timeval *tvp) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getitimer(int, struct itimerval *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setitimer(int, const struct itimerval *, struct itimerval *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
