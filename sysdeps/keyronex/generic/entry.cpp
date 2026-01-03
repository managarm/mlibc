#include <keyronex/syscall.h>

#include <stdint.h>
#include <stdlib.h>

#include <bits/posix/posix_signal.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/debug.hpp>

extern "C" void __dlapi_enter(uintptr_t *);

extern char **environ;

namespace mlibc {

int
sys_sigentry(void *sigentry)
{
	int r = syscall1(SYS_sigentry, (uintptr_t)sigentry, NULL);
	if (r < 0)
		return -r;
	return 0;
}

[[noreturn]] int
sys_sigreturn(ucontext_t *context)
{
	syscall1(SYS_sigreturn, (uintptr_t)context, NULL);
	__builtin_unreachable();
}

} /* namespace mlibc */

static void
__mlibc_sigentry(int which, siginfo_t *siginfo, void *handler,
    bool is_sigaction, ucontext_t *ret_context)
{
	if ((uintptr_t)handler == (uintptr_t)SIG_DFL) {
		mlibc::infoLogger() << "Unhandled signal " << which << frg::endlog;
		mlibc::sys_exit(128 + which);
	} else if ((uintptr_t)handler == (uintptr_t)SIG_IGN) {
		/* epsilon */
	} else {
		if (is_sigaction)
			((void (*)(int, siginfo_t *, void *))handler)(which,
			    siginfo, ret_context);
		else
			((void (*)(int))handler)(which);
	}

	mlibc::sys_sigreturn(ret_context);

	__builtin_unreachable();
}

extern "C" void
__mlibc_entry(int (*main_fn)(int argc, char *argv[], char *env[])) {
	// TODO: call __dlapi_enter, otherwise static builds will break (see
	// Linux sysdeps)
	mlibc::sys_sigentry((void *)__mlibc_sigentry);
	auto result = main_fn(mlibc::entry_stack.argc, mlibc::entry_stack.argv,
	    environ);
	exit(result);
}
