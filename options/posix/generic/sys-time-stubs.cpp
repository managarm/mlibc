
#include <sys/time.h>
#include <bits/ensure.h>

#include <frigg/debug.hpp>

int gettimeofday(struct timeval *__restrict result, void *__restrict unused) {
	frigg::infoLogger() << "mlibc: Broken gettimeofday() called!" << frigg::endLog;
	__ensure(!unused);
	result->tv_sec = 0;
	result->tv_usec = 0;
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

