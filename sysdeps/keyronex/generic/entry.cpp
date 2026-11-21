#include <keyronex/syscall.h>

#include <stdint.h>
#include <stdlib.h>

#include <bits/posix/posix_signal.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/init-priority.hpp>

namespace {

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

} /* namespace */

static void
__mlibc_sigentry(int which, siginfo_t *siginfo, void *handler,
    bool is_sigaction, ucontext_t *ret_context)
{
	if ((uintptr_t)handler == (uintptr_t)SIG_DFL) {
		mlibc::infoLogger() << "Unhandled signal " << which << frg::endlog;
		mlibc::sysdep<Exit>(128 + which);
	} else if ((uintptr_t)handler == (uintptr_t)SIG_IGN) {
		/* epsilon */
	} else {
		if (is_sigaction)
			((void (*)(int, siginfo_t *, void *))handler)(which,
			    siginfo, ret_context);
		else
			((void (*)(int))handler)(which);
	}

	sys_sigreturn(ret_context);

	__builtin_unreachable();
}

// TODO: This might not work properly in static executables,
// and (probably) needs to be moved to generic.cpp or something
[[gnu::constructor(MLIBC_INIT_PRIORITY_ENTRY)]]
static void set_sigentry() {
	sys_sigentry((void *)__mlibc_sigentry);
}
