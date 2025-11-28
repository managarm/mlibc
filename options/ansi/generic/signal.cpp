
#include <bits/ensure.h>
#include <errno.h>
#include <signal.h>

#include <mlibc/debug.hpp>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/sysdeps.hpp>

__sighandler signal(int sn, __sighandler handler) {
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	struct sigaction old;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sigaction, SIG_ERR);
	if(int e = mlibc::sys_sigaction(sn, &sa, &old)){
		errno = e;
		return SIG_ERR;
	}
	return old.sa_handler;
}

int raise(int sig) {
	pid_t pid = sysdeps.getpid();

	if (int e = sysdeps.kill(pid, sig)) {
		errno = e;
		return -1;
	}

	return 0;
}

// This is a POSIX extension, but we have it in here for sigsetjmp
int sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	if(int e = sysdeps.sigprocmask(how, set, retrieve); e) {
		errno = e;
		return -1;
	}
	return 0;
}

