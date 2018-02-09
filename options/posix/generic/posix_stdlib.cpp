
#include <stdlib.h>

#include <bits/ensure.h>

long random(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int setenv(const char *, const char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int unsetenv(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// Path handling.
// ----------------------------------------------------------------------------

int mkstemp(char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *mkdtemp(char *path) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *realpath(const char *__restrict, char *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

