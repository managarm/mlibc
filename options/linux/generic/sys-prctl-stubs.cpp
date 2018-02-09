
#include <sys/prctl.h>

#include <bits/ensure.h>

int prctl (int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

