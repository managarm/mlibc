
#include <langinfo.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>

char *nl_langinfo(nl_item item) {
	return mlibc::nl_langinfo(item);
}

char *nl_langinfo_l(nl_item item, locale_t loc) {
	return mlibc::nl_langinfo_l(item, reinterpret_cast<mlibc::localeinfo *>(loc));
}

