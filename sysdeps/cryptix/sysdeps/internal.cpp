#include <cryptix/syscall.h>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc {
void Sysdeps<LibcLog>::operator()(const char *message) { Syscall(SYS_DEBUG_LOG, message); }
void Sysdeps<LibcPanic>::operator()() {
	Syscall(SYS_PANIC, "mlibc crashed");

	__builtin_unreachable();
}
} // namespace mlibc
