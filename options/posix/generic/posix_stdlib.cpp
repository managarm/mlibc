
#include <stdlib.h>

#include <mlibc/ensure.h>

long random(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

