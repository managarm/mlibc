
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

// TODO: Return EINVAL instead of __ensure()ing.

int sigaddset(sigset_t *sigset, int signo) {
	// TODO: do not hard code CHAR_BITS
	__ensure((unsigned int)signo < sizeof(sigset_t) * 8);
	*sigset |= sigset_t(1) << signo;
	return 0;
}

int sigdelset(sigset_t *sigset, int signo) {
	// TODO: do not hard code CHAR_BITS
	__ensure((unsigned int)signo < sizeof(sigset_t) * 8);
	*sigset &= ~(sigset_t(1) << signo);
	return 0;
}

int sigismember(const sigset_t *set, int signo) {
	// TODO: do not hard code CHAR_BITS
	__ensure((unsigned int)signo < sizeof(sigset_t) * 8);
	return (*set) & (sigset_t(1) << signo);
}

int sigsuspend(const sigset_t *sigmask) {
	__ensure(!"sigsuspend() not implemented");
	__builtin_unreachable();
}

int pthread_sigmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	if(!mlibc::sys_sigprocmask) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}
	if(int e = mlibc::sys_sigprocmask(how, set, retrieve); e) {
		return e;
	}
	return 0;

}

int sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	if(!mlibc::sys_sigprocmask) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_sigprocmask(how, set, retrieve); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sigaction(int signum, const struct sigaction *__restrict act, struct sigaction *__restrict oldact) {
	if(!mlibc::sys_sigaction) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_sigaction(signum, act, oldact); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int kill(pid_t pid, int number) {
	if(!mlibc::sys_kill) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_kill(pid, number); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sigtimedwait(const sigset_t *set, siginfo_t *info, const struct timespec *timeout) {
	__ensure(!"sigtimedwait() not implemented");
	__builtin_unreachable();
}

int sigwait(const sigset_t *set, int *sig) {
	__ensure(!"sigwait() not implemented");
	__builtin_unreachable();
}

