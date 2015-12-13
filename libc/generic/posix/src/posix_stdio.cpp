
#include <stdio.h>

#include <mlibc/ensure.h>

int fileno(FILE *file) {
	return file->fd;
}

FILE *fdopen(int fd, const char *mode) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

