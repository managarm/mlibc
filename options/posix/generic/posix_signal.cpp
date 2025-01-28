
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <bits/ensure.h>

#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/tcb.hpp>

int sigsuspend(const sigset_t *sigmask) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sigsuspend, -1);

	// This is guaranteed to return an error (EINTR most probably)
	errno = mlibc::sys_sigsuspend(sigmask);
	return -1;
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

int pthread_kill(pthread_t thread, int sig) {
	auto tcb = reinterpret_cast<Tcb *>(thread);
	auto pid = getpid();

	if(!mlibc::sys_tgkill) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}

	if(int e = mlibc::sys_tgkill(pid, tcb->tid, sig); e) {
		return e;
	}

	return 0;
}

int sigaction(int signum, const struct sigaction *__restrict act, struct sigaction *__restrict oldact) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sigaction, -1);
	if(int e = mlibc::sys_sigaction(signum, act, oldact); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int siginterrupt(int sig, int flag) {
	int ret;
	struct sigaction act;

	sigaction(sig, NULL, &act);
	if (flag)
		act.sa_flags &= ~SA_RESTART;
	else
		act.sa_flags |= SA_RESTART;

	ret = sigaction(sig, &act, NULL);
	return ret;
}

int kill(pid_t pid, int number) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_kill, -1);
	if(int e = mlibc::sys_kill(pid, number); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int killpg(pid_t pgrp, int sig) {
	if(pgrp > 1) {
		return kill(-pgrp, sig);
	}

	errno = EINVAL;
	return -1;
}

int sigtimedwait(const sigset_t *__restrict set, siginfo_t *__restrict info, const struct timespec *__restrict timeout) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sigtimedwait, -1);

	int signo;

	if (int e = sysdep(set, info, timeout, &signo)) {
		errno = e;
		return -1;
	}

	return signo;
}

int sigwaitinfo(const sigset_t *__restrict set, siginfo_t *__restrict info) {
	// NOTE: This assumes the sysdep behavior noted in mlibc/posix-sysdeps.hpp
	return sigtimedwait(set, info, nullptr);
}

int sigwait(const sigset_t *__restrict set, int *__restrict sig) {
	if (int e = sigwaitinfo(set, nullptr); e < 0) {
		return e;
	} else {
		if (sig)
			*sig = e;

		return 0;
	}
}

int sigpending(sigset_t *set) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sigpending, -1);

	if(int e = sysdep(set)) {
		errno = e;
		return -1;
	}

	return 0;
}

int sigaltstack(const stack_t *__restrict ss, stack_t *__restrict oss) {
	if (ss && ss->ss_size < MINSIGSTKSZ && !(ss->ss_flags & SS_DISABLE)) {
		errno = ENOMEM;
		return -1;
	}

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sigaltstack, -1);
	if (int e = mlibc::sys_sigaltstack(ss, oss); e) {
		errno = e;
		return -1;
	}

	return 0;
}

#if __MLIBC_GLIBC_OPTION
int sigisemptyset(const sigset_t *set) {
	auto ptr = reinterpret_cast<const char *>(set);
	for(size_t i = 0; i < sizeof(sigset_t); i++) {
		if(ptr[i]) {
			return 0;
		}
	}
	return 1;
}
#endif // __MLIBC_GLIBC_OPTION

int sigqueue(pid_t, int, const union sigval) {
	__ensure(!"sigqueue() not implemented");
	__builtin_unreachable();
}

