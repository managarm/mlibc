
#include <bits/ensure.h>
#include <signal.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

void __signalDfl(int signal) {
	__ensure(!"Not implemented");
}
void __signalErr(int signal) {
	__ensure(!"Not implemented");
}
void __signalIgn(int signal) {
	__ensure(!"Not implemented");
}

__sighandler signal(int sn, __sighandler handler) {
	frigg::infoLogger() << "\e[31mmlibc: signal() always returns SIG_DFL\e[39m" << frigg::endLog;
	if(handler == SIG_DFL || handler == SIG_IGN) {
	}else{
		struct sigaction sa;
		sa.sa_handler = handler;
		sa.sa_flags = 0;
		sa.sa_mask = 0;
		if(mlibc::sys_sigaction(sn, &sa, nullptr))
			frigg::panicLogger() << "\e[31mmlibc: sys_sigaction() failed\e[39m" << frigg::endLog;
	}
	return SIG_DFL;
}

int raise(int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

