
#include <errno.h>
#include <sys/wait.h>
#include <bits/ensure.h>

#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/debug.hpp>

int waitid(idtype_t, id_t, siginfo_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

pid_t waitpid(pid_t pid, int *status, int flags) {
	pid_t ret;
	if(!mlibc::sys_waitpid) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_waitpid(pid, status, flags, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}

pid_t wait(int *status) {
	return waitpid(-1, status, 0);
}

pid_t wait3(int *, int, struct rusage *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

pid_t wait4(pid_t pid, int *status, int options, struct rusage *){
	mlibc::infoLogger() << "\e[31mmlibc: wait4() is not implemented correctly\e[39m"
		<< frg::endlog;
	return waitpid(pid, status, options);
}
