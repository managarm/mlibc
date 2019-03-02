
#include <errno.h>
#include <sys/wait.h>
#include <bits/ensure.h>

#include <mlibc/sysdeps.hpp>

int waitid(idtype_t idtype, id_t id, siginfo_t *siginfo, int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

pid_t waitpid(pid_t pid, int *status, int flags) {
	pid_t ret;
	if(int e = mlibc::sys_waitpid(pid, status, flags, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}

pid_t wait(int *status) {
	return waitpid(-1, status, 0);
}
