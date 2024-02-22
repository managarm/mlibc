
#include <errno.h>
#include <sys/wait.h>
#include <bits/ensure.h>

#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/debug.hpp>

int waitid(idtype_t idtype, id_t id, siginfo_t *info, int options) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_waitid, -1);
	if(int e = sysdep(idtype, id, info, options); e) {
		errno = e;
		return -1;
	}
	return 0;
}

pid_t waitpid(pid_t pid, int *status, int flags) {
	pid_t ret;
	int tmp_status = 0;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_waitpid, -1);
	if(int e = mlibc::sys_waitpid(pid, &tmp_status, flags, NULL, &ret); e) {
		errno = e;
		return -1;
	}
	if(status) {
		*status = tmp_status;
	}
	return ret;
}

pid_t wait(int *status) {
	return waitpid(-1, status, 0);
}

pid_t wait3(int *status, int options, struct rusage *rusage) {
	(void) rusage;
	mlibc::infoLogger() << "\e[31mmlibc: wait3() is not implemented correctly\e[39m"
		<< frg::endlog;
	return waitpid(-1, status, options);
}

pid_t wait4(pid_t pid, int *status, int options, struct rusage *ru) {
	pid_t ret;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_waitpid, -1);
	if(int e = mlibc::sys_waitpid(pid, status, options, ru, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}
