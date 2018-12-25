
#include <errno.h>
#include <signal.h>
#include <bits/ensure.h>

#include <mlibc/sysdeps.hpp>

int sigemptyset(sigset_t *sigset) {
	*sigset = 0;
	return 0;
}
int sigfillset(sigset_t *sigset) {
	*sigset = ~sigset_t(0);
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
int sigismember(const sigset_t *set, int signo) {
	__ensure(!"sigismember() not implemented");
	__builtin_unreachable();
}

int sigsuspend(const sigset_t *sigmask) {
	__ensure(!"sigsuspend() not implemented");
	__builtin_unreachable();
}

int pthread_sigmask(int, const sigset_t *__restrict, sigset_t *__restrict) {
	__ensure(!"pthread_sigmask() not implemented");
	__builtin_unreachable();
}

int sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	if(int e = mlibc::sys_sigprocmask(how, set, retrieve); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sigaction(int signum, const struct sigaction *__restrict act, struct sigaction *__restrict oldact) {
	if(int e = mlibc::sys_sigaction(signum, act, oldact); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int kill(pid_t pid, int number) {
	if(int e = mlibc::sys_kill(pid, number); e) {
		errno = e;
		return -1;
	}
	return 0;
}

