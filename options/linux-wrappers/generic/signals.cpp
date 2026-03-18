#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/wrappers.h>

int rt_sigqueueinfo(pid_t pid, int sig, siginfo_t *uinfo) {
	if (int e = mlibc::sysdep_or_enosys<RtSigqueueinfo>(pid, sig, uinfo); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int rt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig, siginfo_t *uinfo) {
	if (int e = mlibc::sysdep_or_enosys<RtTgSigqueueinfo>(tgid, tid, sig, uinfo); e) {
		errno = e;
		return -1;
	}

	return 0;
}
