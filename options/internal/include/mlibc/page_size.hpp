#pragma once
#include <stddef.h>

namespace mlibc {

#if MLIBC_BUILDING_RTLD
[[gnu::visibility("protected")]]
#else
[[gnu::visibility("default")]]
#endif
extern size_t page_size;

} // namespace mlibc
