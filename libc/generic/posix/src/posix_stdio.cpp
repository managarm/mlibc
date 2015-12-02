
#include <stdio.h>

#include <mlibc/ensure.h>

int fileno(FILE *file) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

FILE *fdopen(int fd, const char *mode) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

