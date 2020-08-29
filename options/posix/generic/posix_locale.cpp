#include <bits/posix/posix_locale.h>
#include <bits/ensure.h>

locale_t newlocale(int category_mask, const char *locale, locale_t base) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void freelocale(locale_t locobj) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

