
#include <bits/ensure.h>
#include <sys/timerfd.h>

int timerfd_create(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int timerfd_settime(int, int, const struct itimerspec *, struct itimerspec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int timerfd_gettime(int, struct itimerspec *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

