#include <bits/posix/posix_locale.h>
#include <bits/ensure.h>

locale_t newlocale(int category_mask, const char *locale, locale_t base) {
	// Due to all of the locale functions being stubs, the locale will not be used
	return nullptr;
}

void freelocale(locale_t locobj) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

locale_t uselocale(locale_t locobj) {
	return nullptr;
}

locale_t duplocale(locale_t locobj) {
	return nullptr;
}
