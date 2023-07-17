#include <bits/ensure.h>
#include <bits/posix/posix_signal.h>
#include <keyronex/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.h>
#include <stdint.h>
#include <stdlib.h>

// defined by the POSIX library
void __mlibc_initLocale();

extern "C" uintptr_t *__dlapi_entrystack();

extern char **environ;
static mlibc::exec_stack_data __mlibc_stack_data;

struct LibraryGuard {
	LibraryGuard();
};

static LibraryGuard guard;

LibraryGuard::LibraryGuard()
{
	__mlibc_initLocale();

	// Parse the exec() stack.
	mlibc::parse_exec_stack(__dlapi_entrystack(), &__mlibc_stack_data);
	mlibc::set_startup_data(__mlibc_stack_data.argc,
	    __mlibc_stack_data.argv, __mlibc_stack_data.envp);
}

namespace mlibc {
int
sys_sigentry(void *sigentry)
{
	uintptr_t ret = syscall1(kPXSysSigEntry, (uintptr_t)sigentry, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

[[noreturn]] int
sys_sigreturn(ucontext_t *context)
{
	syscall1(kPXSysSigReturn, (uintptr_t)context, NULL);
	__builtin_unreachable();
}
}

static void
do_stacktrace(ucontext_t *ctx)
{
	size_t *base_ptr = (size_t *)ctx->uc_mcontext.gregs[REG_RBP];

	mlibc::infoLogger() << "Stacktrace:" << frg::endlog;
	mlibc::infoLogger() << "  [" << (void *)ctx->uc_mcontext.gregs[REG_RIP]
			    << "]" << frg::endlog;
	for (;;) {
		size_t old_bp = base_ptr[0];
		size_t ret_addr = base_ptr[1];
		if (!ret_addr)
			break;
		mlibc::infoLogger()
		    << "  [" << (void *)ret_addr << "]" << frg::endlog;
		if (!old_bp)
			break;
		base_ptr = (size_t *)old_bp;
	}
}

static void
__mlibc_sigentry(int which, siginfo_t *siginfo, void *handler,
    bool is_sigaction, ucontext_t *ret_context)
{
	if ((uintptr_t)handler == (uintptr_t)SIG_DFL) {
		mlibc::infoLogger()
		    << "mlibc: Unhandled signal " << which << frg::endlog;
		do_stacktrace(ret_context);
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
__mlibc_entry(int (*main_fn)(int argc, char *argv[], char *env[]))
{
	/* communicate the signal handler entry point to the kernel */
	mlibc::sys_sigentry((void *)__mlibc_sigentry);

	// TODO: call __dlapi_enter, otherwise static builds will break (see
	// Linux sysdeps)
	auto result = main_fn(__mlibc_stack_data.argc, __mlibc_stack_data.argv,
	    environ);
	exit(result);
}
