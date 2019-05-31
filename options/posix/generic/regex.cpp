#include <regex.h>

#include <bits/ensure.h>

int regcomp(regex_t *__restrict, const char *__restrict, int) {
	__ensure(!"Function is not implemented");
	__builtin_unreachable();
}

int regexec(const regex_t *__restrict, const char *__restrict, size_t, regmatch_t *__restrict, int) {
	__ensure(!"Function is not implemented");
	__builtin_unreachable();
}

size_t regerror(int, const regex_t *__restrict, char *__restrict, size_t) {
	__ensure(!"Function is not implemented");
	__builtin_unreachable();
}

void regfree(regex_t *) {
	__ensure(!"Function is not implemented");
	__builtin_unreachable();
}
