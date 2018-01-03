
#include <fnmatch.h>
#include <mlibc/ensure.h>

int fnmatch(const char *, const char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

