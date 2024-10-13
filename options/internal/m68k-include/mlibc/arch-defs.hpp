#ifndef MLIBC_ARCH_DEFS_HPP
#define MLIBC_ARCH_DEFS_HPP

#include <stddef.h>

namespace mlibc {

// not strictly true, can be 4 or 8k on 68040/68060, and many more on others
inline constexpr size_t page_size = 0x1000;

} // namespace mlibc

#endif // MLIBC_ARCH_DEFS_HPP
