#include <keyronex/syscall.h>

#include <mlibc/all-sysdeps.hpp>

namespace mlibc {

[[noreturn]] void
Sysdeps<Exit>::operator()(int status)
{
	(void)status;
	syscall1(SYS_exit, status, NULL);
	sysdep<LibcPanic>();
}

int
Sysdeps<Fork>::operator()(pid_t *child)
{
	pid_t ret = syscall0(SYS_fork, NULL);
	if (ret < 0)
		return -ret;

	*child = ret;
	return 0;
}

int
Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[])
{
	int ret = syscall3(SYS_execve, (uintptr_t)path, (uintptr_t)argv, (uintptr_t)envp, NULL);
	return -ret;
}

int
Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid)
{
	pid_t ret = syscall4(SYS_wait4, pid, (uintptr_t)status, flags, (uintptr_t)ru, NULL);
	if (ret < 0)
		return -ret;
	*ret_pid = ret;
	return 0;
}

pid_t
Sysdeps<GetPid>::operator()()
{
	return syscall0(SYS_getpid, NULL);
}

pid_t
Sysdeps<GetPpid>::operator()()
{
	return syscall0(SYS_getppid, NULL);
}

/* job control */

int
Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid_out)
{
	pid_t pgid = syscall1(SYS_getpgid, pid, NULL);
	if (pgid < 0)
		return -pgid;
	*pgid_out = pgid;
	return 0;
}

int
Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid)
{
	int r = syscall2(SYS_setpgid, pid, pgid, NULL);
	if(r < 0)
		return -r;
	return 0;
}

int
Sysdeps<GetSid>::operator()(pid_t pid, pid_t *sid)
{
	pid_t s = syscall1(SYS_getsid, pid, NULL);
	if (s < 0)
		return -s;
	*sid = s;
	return 0;
}

int
Sysdeps<SetSid>::operator()(pid_t *sid)
{
	pid_t s = syscall0(SYS_setsid, NULL);
	if (s < 0)
		return -s;
	*sid = s;
	return 0;
}

} /* namespace mlibc */
