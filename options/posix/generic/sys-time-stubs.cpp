
#include <sys/time.h>

#include <bits/ensure.h>

// gettimeofday() is provided by the platform

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

