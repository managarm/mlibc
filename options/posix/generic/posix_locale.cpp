#include <bits/posix/posix_locale.h>
#include <bits/ensure.h>

locale_t newlocale(int, const char *, locale_t) {
	// Due to all of the locale functions being stubs, the locale will not be used
	return nullptr;
}

void freelocale(locale_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

locale_t uselocale(locale_t) {
	return nullptr;
}

locale_t duplocale(locale_t) {
	return nullptr;
}
