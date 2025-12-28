
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
 	if(mlibc::sys_thread_sigmask) {
	 	if(int e = mlibc::sys_thread_sigmask(how, set, retrieve); e) {
	 	 	return e;
	 	}
 	 	return 0;
 	}

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

	sigaction(sig, nullptr, &act);
	if (flag)
		act.sa_flags &= ~SA_RESTART;
	else
		act.sa_flags |= SA_RESTART;

	ret = sigaction(sig, &act, nullptr);
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

int sigqueue(pid_t pid, int sig, const union sigval val) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sigqueue, -1);
	if (int e = mlibc::sys_sigqueue(pid, sig, val); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int sig2str(int signum, char *str) {
	const char *name = nullptr;

	if (signum > SIGRTMIN && signum < SIGRTMAX) {
		snprintf(str, SIG2STR_MAX, "RTMIN+%d", signum - SIGRTMIN);
		return 0;
	}

#define CASE_FOR(x) case SIG##x: name = #x; break;

	switch (signum) {
		CASE_FOR(RTMIN)
		CASE_FOR(RTMAX)

		CASE_FOR(HUP)
		CASE_FOR(INT)
		CASE_FOR(QUIT)
		CASE_FOR(ILL)
		CASE_FOR(TRAP)
		CASE_FOR(ABRT)
		CASE_FOR(BUS)
		CASE_FOR(FPE)
		CASE_FOR(KILL)
		CASE_FOR(USR1)
		CASE_FOR(SEGV)
		CASE_FOR(USR2)
		CASE_FOR(PIPE)
		CASE_FOR(ALRM)
		CASE_FOR(TERM)
#ifdef SIGSTKFLT
		CASE_FOR(STKFLT)
#endif
		CASE_FOR(CHLD)
		CASE_FOR(CONT)
		CASE_FOR(STOP)
		CASE_FOR(TSTP)
		CASE_FOR(TTIN)
		CASE_FOR(TTOU)
		CASE_FOR(URG)
		CASE_FOR(XCPU)
		CASE_FOR(XFSZ)
		CASE_FOR(VTALRM)
		CASE_FOR(PROF)
		CASE_FOR(WINCH)
		CASE_FOR(POLL)
		CASE_FOR(PWR)
		CASE_FOR(SYS)
		CASE_FOR(CANCEL)
#ifdef SIGTIMER
		CASE_FOR(TIMER)
#endif

#undef CASE_FOR

		default:
			return -1;
	}

	strlcpy(str, name, SIG2STR_MAX);
	return 0;
}

int str2sig(const char *__restrict str, int *__restrict pnum) {
#define CASE_FOR(x) if (!strcmp(str, #x)) { *pnum = SIG##x; return 0; }

	CASE_FOR(RTMIN)
	CASE_FOR(RTMAX)

	CASE_FOR(HUP)
	CASE_FOR(INT)
	CASE_FOR(QUIT)
	CASE_FOR(ILL)
	CASE_FOR(TRAP)
	CASE_FOR(ABRT)
	CASE_FOR(BUS)
	CASE_FOR(FPE)
	CASE_FOR(KILL)
	CASE_FOR(USR1)
	CASE_FOR(SEGV)
	CASE_FOR(USR2)
	CASE_FOR(PIPE)
	CASE_FOR(ALRM)
	CASE_FOR(TERM)
#ifdef SIGSTKFLT
	CASE_FOR(STKFLT)
#endif
	CASE_FOR(CHLD)
	CASE_FOR(CONT)
	CASE_FOR(STOP)
	CASE_FOR(TSTP)
	CASE_FOR(TTIN)
	CASE_FOR(TTOU)
	CASE_FOR(URG)
	CASE_FOR(XCPU)
	CASE_FOR(XFSZ)
	CASE_FOR(VTALRM)
	CASE_FOR(PROF)
	CASE_FOR(WINCH)
	CASE_FOR(POLL)
	CASE_FOR(PWR)
	CASE_FOR(SYS)
	CASE_FOR(CANCEL)
#ifdef SIGTIMER
	CASE_FOR(TIMER)
#endif

#undef CASE_FOR

	if (!strncmp(str, "RTMIN+", 6)) {
		char *endptr = nullptr;
		errno = 0;
		auto offset = strtol(str + 6, &endptr, 10);

		if (errno || *endptr != '\0' || offset < 0 || (SIGRTMIN + offset >= SIGRTMAX))
			return -1;

		*pnum = SIGRTMIN + offset;
		return 0;
	} else if (!strncmp(str, "RTMAX-", 6)) {
		char *endptr = nullptr;
		errno = 0;
		auto offset = strtol(str + 6, &endptr, 10);

		if (errno || *endptr != '\0' || offset < 0 || (SIGRTMAX - offset <= SIGRTMIN))
			return -1;

		*pnum = SIGRTMAX - offset;
		return 0;
	}

	return -1;
}

void psiginfo(const siginfo_t *pinfo, const char *message) {
	psignal(pinfo->si_signo, message);
}

void psignal(int signum, const char *message) {
	auto name = strsignal(signum);

	flockfile(stderr);
	auto save_mode = stderr->__io_mode;

	fprintf(stderr, "%s%s%s\n", message ? message : "", message ? ": " : "", name);

	stderr->__io_mode = save_mode;
	funlockfile(stderr);
}
