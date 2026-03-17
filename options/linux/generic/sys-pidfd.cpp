#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <sys/pidfd.h>

int pidfd_open(pid_t pid, unsigned int flags) {
	int fd = 0;

	if(int e = mlibc::sysdep_or_enosys<PidfdOpen>(pid, flags, &fd); e) {
		errno = e;
		return -1;
	}

	return fd;
}

pid_t pidfd_getpid(int fd) {
	pid_t pid = 0;

	if(int e = mlibc::sysdep_or_enosys<PidfdGetpid>(fd, &pid); e) {
		errno = e;
		return -1;
	}

	return pid;
}

int pidfd_send_signal(int pidfd, int sig, siginfo_t *info, unsigned int flags) {
	if(int e = mlibc::sysdep_or_enosys<PidfdSendSignal>(pidfd, sig, info, flags); e) {
		errno = e;
		return -1;
	}

	return 0;
}
