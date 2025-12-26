#include <mlibc/exit.hpp>

namespace mlibc {
std::atomic<bool> processIsExiting = false;
} // namespace mlibc
