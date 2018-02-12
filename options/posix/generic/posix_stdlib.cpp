
#include <stdlib.h>

#include <bits/ensure.h>

long random(void) {
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

