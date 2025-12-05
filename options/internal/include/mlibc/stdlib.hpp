#pragma once

namespace mlibc {

int mkostemps(char *pattern, int suffixlen, int flags, int *fd);

} // namespace mlibc
