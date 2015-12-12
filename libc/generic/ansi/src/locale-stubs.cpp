
#include <locale.h>

#include <mlibc/ensure.h>

char *setlocale(int category, const char *locale) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

struct lconv *localeconv(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

