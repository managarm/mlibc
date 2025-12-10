#include <bits/posix/posix_locale.h>
#include <bits/ensure.h>
#include <frg/allocation.hpp>
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

locale_t duplocale(locale_t loc) {
	if (loc == LC_GLOBAL_LOCALE)
		return frg::construct<mlibc::localeinfo>(getAllocator(), *mlibc::getGlobalLocale());

	return frg::construct<mlibc::localeinfo>(getAllocator(), *reinterpret_cast<mlibc::localeinfo *>(loc));
}

const char *getlocalename_l(int category, locale_t loc) {
	if (category < LC_CTYPE || category > LC_IDENTIFICATION || !loc) {
		return nullptr;
	}

	mlibc::localeinfo *l;
	if (loc == LC_GLOBAL_LOCALE)
		l = mlibc::getGlobalLocale();
	else
		l = static_cast<mlibc::localeinfo *>(loc);
	return l->getCategoryLocaleName(category).data();
}
