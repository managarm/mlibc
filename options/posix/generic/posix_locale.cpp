#include <bits/posix/posix_locale.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>

locale_t newlocale(int, const char *, locale_t) {
	// Due to all of the locale functions being stubs, the locale will not be used
	mlibc::infoLogger() << "mlibc: newlocale() is a no-op" << frg::endlog;
	return nullptr;
}

void freelocale(locale_t) {
	mlibc::infoLogger() << "mlibc: freelocale() is a no-op" << frg::endlog;
	return;
}

locale_t uselocale(locale_t) {
	mlibc::infoLogger() << "mlibc: uselocale() is a no-op" << frg::endlog;
	return nullptr;
}

locale_t duplocale(locale_t) {
	mlibc::infoLogger() << "mlibc: duplocale() is a no-op" << frg::endlog;
	return nullptr;
}
