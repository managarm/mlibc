
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <bits/ensure.h>
#include <frigg/debug.hpp>

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
	frigg::infoLogger() << "\e[31mmlibc: open_memstream() always fails"
			<< "\e[39m" << frigg::endLog;
	errno = ENOMEM;
	return nullptr;
}

int fseeko(FILE *stream, off_t offset, int whence) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

off_t ftello(FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

