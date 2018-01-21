
#include <stdio.h>

#include <mlibc/ensure.h>

int fileno(FILE *file) {
	return file->fd;
}

FILE *fdopen(int fd, const char *mode) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

FILE *fmemopen(void *__restrict, size_t, const char *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pclose(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

FILE *popen(const char*, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

FILE *open_memstream(char **, size_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

