
#include <bits/ensure.h>
#include <frg/string.hpp>
#include <mlibc/charset.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>
#include <mlibc/wide.hpp>
#include <wctype.h>

wctrans_t wctrans(const char *property) {
	return find_wctrans(property, mlibc::getActiveLocale());
}

wint_t towctrans(wint_t wc, wctrans_t wct) {
	return mlibc::current_charset()->towctrans(wc, wct, mlibc::getActiveLocale());
}
