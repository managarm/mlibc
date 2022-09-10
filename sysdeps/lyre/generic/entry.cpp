#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.h>
#include <mlibc/all-sysdeps.hpp>
#include <bits/posix/posix_signal.h>
#include <lyre/syscall.h>

// defined by the POSIX library
void __mlibc_initLocale();

extern "C" uintptr_t *__dlapi_entrystack();

extern char **environ;
static mlibc::exec_stack_data __mlibc_stack_data;

struct LibraryGuard {
	LibraryGuard();
};

static LibraryGuard guard;

LibraryGuard::LibraryGuard() {
	__mlibc_initLocale();

	// Parse the exec() stack.
	mlibc::parse_exec_stack(__dlapi_entrystack(), &__mlibc_stack_data);
	mlibc::set_startup_data(__mlibc_stack_data.argc, __mlibc_stack_data.argv,
			__mlibc_stack_data.envp);
}

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
	int sys_sigentry(void *sigentry) {
		__syscall_ret ret = __syscall(27, sigentry);
		if (ret.errno != 0)
			return ret.errno;
		return 0;
	}

	[[noreturn]] int sys_sigreturn(void *context, sigset_t old_mask) {
		__syscall(30, context, old_mask);
		__builtin_unreachable();
	}
}

static void __mlibc_sigentry(int which, siginfo_t *siginfo,
		void (*sa)(int, siginfo_t *, void *),
		GPRState *ret_context, sigset_t prev_mask) {

/*
	size_t *base_ptr = (size_t *)ret_context->rbp;

	mlibc::infoLogger() << "Stacktrace:" << frg::endlog;
	mlibc::infoLogger() << "  [" << (void *)ret_context->rip << "]" << frg::endlog;
	for (;;) {
		size_t old_bp = base_ptr[0];
		size_t ret_addr = base_ptr[1];
		if (!ret_addr)
			break;
		size_t off;
		mlibc::infoLogger() << "  [" << (void *)ret_addr << "]" << frg::endlog;
		if (!old_bp)
			break;
		base_ptr = (size_t *)old_bp;
	}
*/

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

extern "C" void __mlibc_entry(int (*main_fn)(int argc, char *argv[], char *env[])) {
	//mlibc::sys_sigentry((void *)__mlibc_sigentry);

	// TODO: call __dlapi_enter, otherwise static builds will break (see Linux sysdeps)
	auto result = main_fn(__mlibc_stack_data.argc, __mlibc_stack_data.argv, environ);
	exit(result);
}

