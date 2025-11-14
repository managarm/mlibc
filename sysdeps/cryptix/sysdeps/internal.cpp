#include <cstddef>
#include <stdio.h>
#include <stdlib.h>

#include <cryptix/syscall.h>

#include <execinfo.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <sys/mman.h>

namespace mlibc {
static void do_log(const char *message, size_t length) { Syscall(SYS_DEBUG_LOG, message, length); }
void sys_libc_log(const char *message) {
	static constexpr char MLIBC_SIG[] = "[mlibc]: ";

	size_t len = strlen(message);

	long nwritten = 0;
	do_log(MLIBC_SIG, sizeof(MLIBC_SIG));
	do_log(message, len);
	do_log("\n", 1);
}
[[noreturn]] void sys_libc_panic() {
	Syscall(SYS_PANIC, "mlibc crashed");

	__builtin_unreachable();
}
} // namespace mlibc
