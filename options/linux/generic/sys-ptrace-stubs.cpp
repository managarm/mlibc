
#include <sys/ptrace.h>

#include <bits/ensure.h>

long ptrace(int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

