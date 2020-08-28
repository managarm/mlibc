
#include <bits/ensure.h>
#include <errno.h>
#include <signal.h>

#include <mlibc/debug.hpp>
#include <mlibc/ansi-sysdeps.hpp>

__sighandler signal(int sn, __sighandler handler) {
	if(!mlibc::sys_sigaction) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return SIG_ERR;
	}
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sa.sa_mask = 0;
	struct sigaction old;
	if(mlibc::sys_sigaction(sn, &sa, &old))
		mlibc::panicLogger() << "\e[31mmlibc: sys_sigaction() failed\e[39m" << frg::endlog;
	return old.sa_handler;
}

int raise(int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

