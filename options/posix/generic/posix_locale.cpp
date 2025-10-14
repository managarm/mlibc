#include <bits/posix/posix_locale.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>

locale_t newlocale(int category, const char *name, locale_t base) {
	mlibc::localeinfo *loc = static_cast<mlibc::localeinfo *>(base);
	if (int e = mlibc::loadLocale(category, name, &loc); e) {
		errno = e;
		return nullptr;
	}

	return loc;
}

void freelocale(locale_t loc) {
	mlibc::freeLocale(reinterpret_cast<mlibc::localeinfo *>(loc));
}

locale_t uselocale(locale_t loc) {
	return mlibc::useThreadLocalLocale(reinterpret_cast<mlibc::localeinfo *>(loc));
}

locale_t duplocale(locale_t) {
	mlibc::infoLogger() << "mlibc: duplocale() is a no-op" << frg::endlog;
	return nullptr;
}
