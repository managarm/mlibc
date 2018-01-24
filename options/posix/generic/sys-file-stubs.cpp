
#include <sys/file.h>

#include <bits/ensure.h>

int flock(int, int) {
	__ensure(!"flock() not implemented");
	__builtin_unreachable();
}

