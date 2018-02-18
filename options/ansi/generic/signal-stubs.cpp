
#include <bits/ensure.h>
#include <signal.h>

#include <frigg/debug.hpp>

void __signalDfl(int signal) {
	__ensure(!"Not implemented");
}
void __signalErr(int signal) {
	__ensure(!"Not implemented");
}
void __signalIgn(int signal) {
	__ensure(!"Not implemented");
}

__sighandler signal(int sig, __sighandler handler) {
	frigg::infoLogger() << "\e[31mmlibc: signal() always returns SIG_DFL\e[39m" << frigg::endLog;
	return SIG_DFL;
}

int raise(int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

