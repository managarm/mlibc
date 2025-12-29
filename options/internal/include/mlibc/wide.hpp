#pragma once

#include <mlibc/locale.hpp>
#include <wctype.h>

namespace mlibc {

wctrans_t find_wctrans(frg::string_view name, mlibc::localeinfo *l);

} // namespace mlibc
