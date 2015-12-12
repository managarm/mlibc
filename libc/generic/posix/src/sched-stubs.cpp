
#include <sched.h>

#include <mlibc/ensure.h>

int sched_yield() {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

