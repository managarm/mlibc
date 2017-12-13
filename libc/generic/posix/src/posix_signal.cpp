
#include <signal.h>

#include <mlibc/ensure.h>

int sigemptyset(sigset_t *sigset) {
	*sigset = 0;
	return 0;
}
int sigfillset(sigset_t *) {
	__ensure(!"sigfillset() not implemented");
	__builtin_unreachable();
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
	__ensure(!"sigsuspend() not implemented");
	__builtin_unreachable();
}

int pthread_sigmask(int, const sigset_t *__restrict, sigset_t *__restrict) {
	__ensure(!"pthread_sigmask() not implemented");
	__builtin_unreachable();
}
// sigprocmask() is provided by the platform

// sigaction() is provided by the platform

int kill(pid_t pid, int sig) {
	__ensure(!"kill() not implemented");
	__builtin_unreachable();
}

