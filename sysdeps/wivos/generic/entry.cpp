#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.h>
#include <mlibc/all-sysdeps.hpp>
#include <bits/posix/posix_signal.h>

extern "C" void __dlapi_enter(uintptr_t *);

extern char **environ;

struct GPRState {
	uint64_t ds;
	uint64_t es;
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t err;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};

namespace mlibc {
	[[noreturn]] int sys_sigreturn(void *context, sigset_t old_mask) {
        (void)context;
        (void)old_mask;
        //TODO
        while(1);
		//__syscall(30, context, old_mask);
		//__builtin_unreachable();
	}
}

static void __mlibc_sigentry(int which, siginfo_t *siginfo,
		void (*sa)(int, siginfo_t *, void *),
		GPRState *ret_context, sigset_t prev_mask) {
	switch ((uintptr_t)sa) {
		// DFL
		case (uintptr_t)(-2):
			mlibc::infoLogger() << "mlibc: Unhandled signal " << which << frg::endlog;
			mlibc::sys_exit(128 + which);
		// IGN
		case (uintptr_t)(-3):
			break;
		default:
			sa(which, siginfo, NULL);
			break;
	}

	mlibc::sys_sigreturn(ret_context, prev_mask);

	__builtin_unreachable();
}

extern "C" void __mlibc_entry(uintptr_t *entry_stack, int (*main_fn)(int argc, char *argv[], char *env[])) {
	__dlapi_enter(entry_stack);

	auto result = main_fn(mlibc::entry_stack.argc, mlibc::entry_stack.argv, environ);
	exit(result);
}
