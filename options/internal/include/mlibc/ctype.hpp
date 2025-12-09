#pragma once

#include <mlibc/locale.hpp>

namespace mlibc {

int isalpha_l(int c, localeinfo *l);
int isdigit_l(int c, localeinfo *l);
int isspace_l(int c, localeinfo *l);
int isxdigit_l(int c, localeinfo *l);

int tolower_l(int c, localeinfo *l);

} // namespace mlibc
