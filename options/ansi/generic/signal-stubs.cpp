
#include <bits/ensure.h>
#include <errno.h>
#include <signal.h>

#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

__sighandler signal(int sn, __sighandler handler) {
	mlibc::infoLogger() << "\e[31mmlibc: signal() always returns SIG_DFL\e[39m" << frg::endlog;
	if(!mlibc::sys_sigaction) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return SIG_ERR;
	}
	if(handler == SIG_DFL || handler == SIG_IGN) {
	}else{
		struct sigaction sa;
		sa.sa_handler = handler;
		sa.sa_flags = 0;
		sa.sa_mask = 0;
		if(mlibc::sys_sigaction(sn, &sa, nullptr))
			mlibc::panicLogger() << "\e[31mmlibc: sys_sigaction() failed\e[39m" << frg::endlog;
	}
	return SIG_DFL;
}

int raise(int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

