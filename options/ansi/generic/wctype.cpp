
#include <bits/ensure.h>
#include <frg/string.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/locale.hpp>
#include <mlibc/wide.hpp>
#include <wctype.h>

wctrans_t wctrans(const char *property) {
	return find_wctrans(property, mlibc::getActiveLocale());
}

wint_t towctrans(wint_t, wctrans_t) { MLIBC_STUB_BODY; }

