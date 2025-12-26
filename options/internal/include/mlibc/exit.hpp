#ifndef MLIBC_EXIT_HPP
#define MLIBC_EXIT_HPP

#include <atomic>

namespace mlibc {
extern std::atomic<bool> processIsExiting;
} // namespace mlibc

#endif
