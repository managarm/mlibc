
#include <bits/ensure.h>
#include <sys/signalfd.h>

int signalfd(int, const sigset_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

