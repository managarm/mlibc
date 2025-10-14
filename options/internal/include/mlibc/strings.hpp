#ifndef MLIBC_STRINGS
#define MLIBC_STRINGS

#include <bits/size_t.h>

namespace mlibc {

int strncasecmp(const char *a, const char *b, size_t size);
size_t strnlen(const char *s, size_t n);

} // namespace mlibc

#endif // MLIBC_STRINGS
