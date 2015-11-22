
#include <signal.h>

#include <mlibc/ensure.h>

typedef int sig_atomic_t;
typedef void (*__sighandler) (int);

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
}

int raise(int sig) {
	__ensure(!"Not implemented");
}

