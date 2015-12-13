
#include <locale.h>

#include <mlibc/ensure.h>

#pragma GCC visibility push(hidden)

#include <frigg/debug.hpp>

#pragma GCC visibility pop

char *setlocale(int category, const char *locale) {
	frigg::infoLogger.log() << "mlibc: Broken setlocale(\""
			<< locale << "\") called!" << frigg::EndLog();
	return "C";
}

struct lconv *localeconv(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

