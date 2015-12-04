
#include <string.h>

#include <mlibc/ensure.h>

char *strdup(const char *string) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

