
#include <bits/ensure.h>
#include <errno.h>
#include <signal.h>

#include <mlibc/debug.hpp>
#include <mlibc/ansi-sysdeps.hpp>

__sighandler signal(int sn, __sighandler handler) {
	if(!mlibc::sys_sigaction) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return SIG_ERR;
	}
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sa.sa_mask = 0;
	struct sigaction old;
	if(mlibc::sys_sigaction(sn, &sa, &old))
		mlibc::panicLogger() << "\e[31mmlibc: sys_sigaction() failed\e[39m" << frg::endlog;
	return old.sa_handler;
}

int raise(int sig) {
	if(!mlibc::sys_getpid || !mlibc::sys_kill) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	pid_t pid = mlibc::sys_getpid();

	if (int e = mlibc::sys_kill(pid, sig)) {
		errno = e;
		return -1;
	}

	return 0;
}

// This is a POSIX extension, but we have it in here for sigsetjmp
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

