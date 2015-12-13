
#include <signal.h>

#include <mlibc/ensure.h>

int sigemptyset(sigset_t *sigset) {
	*sigset = 0;
	return 0;
}
int sigaddset(sigset_t *sigset, int sig) {
	// TODO: do not hard code CHAR_BITS
	__ensure((unsigned int)sig < sizeof(sigset_t) * 8);
	*sigset |= sigset_t(1) << sig;
	return 0;
}
int sigdelset(sigset_t *sigset, int sig) {
	// TODO: do not hard code CHAR_BITS
	__ensure((unsigned int)sig < sizeof(sigset_t) * 8);
	*sigset &= ~(sigset_t(1) << sig);
	return 0;
}

int sigsuspend(const sigset_t *sigmask) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// sigprocmask() is provided by the platform

// sigaction() is provided by the platform

int kill(pid_t pid, int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

