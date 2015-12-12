
#include <sys/time.h>

#include <mlibc/ensure.h>

int gettimeofday(struct timeval *__restrict result, void *__restrict unused) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

