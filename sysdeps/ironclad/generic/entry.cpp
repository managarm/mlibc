
#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>
#include <sys/syscall.h>
#include <mlibc/debug.hpp>

extern "C" void __dlapi_enter(uintptr_t *);

extern char **environ;

extern "C" void __mlibc_sigret(void) {
	int ret, errno;
	SYSCALL0(SYSCALL_SIGNAL_RETURN);
	mlibc::panicLogger() << "mlibc: failed to exit signal with " << errno << frg::endlog;
	__builtin_unreachable();
}

extern "C" void __mlibc_entry(uintptr_t *entry_stack, int (*main_fn)(int argc, char *argv[], char *env[])) {
	__dlapi_enter(entry_stack);

	auto result = main_fn(mlibc::entry_stack.argc, mlibc::entry_stack.argv, environ);
	exit(result);
}

