#include <bits/posix/posix_locale.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>

locale_t newlocale(int category, const char *name, locale_t base) {
	return mlibc::loadLocale(category, name, base);
}

void freelocale(locale_t loc) {
	mlibc::freeLocale(loc);
}

locale_t uselocale(locale_t loc) {
	return mlibc::useThreadLocalLocale(loc);
}

locale_t duplocale(locale_t) {
	mlibc::infoLogger() << "mlibc: duplocale() is a no-op" << frg::endlog;
	return nullptr;
}
