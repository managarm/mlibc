#pragma once

#include <mlibc/locale.hpp>
#include <wctype.h>

namespace mlibc {

wctrans_t find_wctrans(frg::string_view name, mlibc::localeinfo *l);
wchar_t *wcpncpy(wchar_t *__restrict ws1, const wchar_t *__restrict ws2, size_t n);

} // namespace mlibc
