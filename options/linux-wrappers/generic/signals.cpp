#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/linux-wrappers-sysdeps.hpp>
#include <mlibc/wrappers.h>

int rt_sigqueueinfo(pid_t pid, int sig, siginfo_t *uinfo) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_rt_sigqueueinfo, ENOSYS);
	if (int e = sysdep(pid, sig, uinfo); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int rt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig, siginfo_t *uinfo) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_rt_tgsigqueueinfo, ENOSYS);
	if (int e = sysdep(tgid, tid, sig, uinfo); e) {
		errno = e;
		return -1;
	}

	return 0;
}
