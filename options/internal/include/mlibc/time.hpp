#pragma once

#include <bits/size_t.h>
#include <mlibc/locale.hpp>
#include <time.h>

namespace mlibc {

size_t strftime(
    char *__restrict dest,
    size_t max_size,
    const char *__restrict format,
    const struct tm *__restrict tm,
    localeinfo *l
);

} // namespace mlibc
