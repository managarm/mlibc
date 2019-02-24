
#include <sys/times.h>

#include <bits/ensure.h>

clock_t times(struct tms *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

