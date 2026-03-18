
#include <bits/ensure.h>
#include <errno.h>
#include <signal.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

__sighandler signal(int sn, __sighandler handler) {
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	struct sigaction old;
	if(int e = mlibc::sysdep_or_enosys<Sigaction>(sn, &sa, &old)){
		errno = e;
		return SIG_ERR;
	}
	return old.sa_handler;
}

int raise(int sig) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<GetPid> && mlibc::IsImplemented<Kill>, -1);
	pid_t pid = mlibc::sysdep_or_panic<GetPid>();

	if (int e = mlibc::sysdep_or_panic<Kill>(pid, sig)) {
		errno = e;
		return -1;
	}

	return 0;
}

// This is a POSIX extension, but we have it in here for sigsetjmp
int sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	if(int e = mlibc::sysdep_or_enosys<Sigprocmask>(how, set, retrieve); e) {
		errno = e;
		return -1;
	}
	return 0;
}

