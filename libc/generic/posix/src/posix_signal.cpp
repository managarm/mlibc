
#include <signal.h>

#include <mlibc/ensure.h>

int sigemptyset(sigset_t *sigset) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int sigaddset(sigset_t *sigset, int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int sigdelset(sigset_t *sigset, int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sigsuspend(const sigset_t *sigmask) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int sigprocmask(int, const sigset_t *__restrict, const sigset_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int kill(pid_t pid, int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

