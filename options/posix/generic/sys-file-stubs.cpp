
#include <sys/file.h>

#include <mlibc/ensure.h>

int flock(int, int) {
	__ensure(!"flock() not implemented");
	__builtin_unreachable();
}

