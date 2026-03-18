#include <sys/types.h>

#include <keyronex/syscall.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {

int
Sysdeps<Kill>::operator()(pid_t pid, int signal)
{
	int r = syscall2(SYS_kill, pid, signal, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Sigaction>::operator()(int signal, const struct sigaction *__restrict action,
    struct sigaction *__restrict oldAction)
{
	int r = syscall3(SYS_sigaction, signal, (uintptr_t)action,
	    (uintptr_t)oldAction, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Sigprocmask>::operator()(int how, const sigset_t *__restrict set,
    sigset_t *__restrict retrieve)
{
	int r = syscall3(SYS_sigprocmask, how, (uintptr_t)set,
	    (uintptr_t)retrieve, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Sigsuspend>::operator()(const sigset_t *set)
{
	int r = syscall1(SYS_sigsuspend, (uintptr_t)set, NULL);
	if (r < 0)
		return -r;

	mlibc::panicLogger() << "Unexpected zero return from sigsuspend()" <<
	    frg::endlog;

	return 0;
}

}
