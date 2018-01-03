
#include <glob.h>
#include <mlibc/ensure.h>

int glob(const char *__restirct, int, int(*)(const char *, int), struct glob_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void globfree(struct glob_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

