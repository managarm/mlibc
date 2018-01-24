
#include <signal.h>

#include <bits/ensure.h>

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
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int raise(int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

