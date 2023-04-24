#include <sys/types.h>

#include <keyronex/syscall.h>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {

int
sys_sigprocmask(int how, const sigset_t *__restrict set,
    sigset_t *__restrict retrieve)
{
	auto ret = syscall3(kPXSysSigMask, how, (uintptr_t)set,
	    (uintptr_t)retrieve, NULL);
	if (int e = sc_error(ret); e) {
		return e;
	}

	return 0;
}

int
sys_sigaction(int signal, const struct sigaction *__restrict action,
    struct sigaction *__restrict oldAction)
{
	auto ret = syscall3(kPXSysSigAction, signal, (uintptr_t)action,
	    (uintptr_t)oldAction, NULL);
	if (int e = sc_error(ret); e) {
		return e;
	}

	return 0;
}

int
sys_kill(int pid, int signal)
{
	if (signal == 0) {
		mlibc::infoLogger() << "Sending signal 0! Allowing" << frg::endlog;
		return 0;
	}

	auto ret = syscall2(kPXSysSigSend, pid, signal, NULL);
	if (int e = sc_error(ret); e) {
		return e;
	}

	return 0;
}

int
sys_sigsuspend(const sigset_t *set)
{
	auto ret = syscall1(kPXSysSigSuspend, (uintptr_t)set, NULL);
	if (int e = sc_error(ret); e) {
		return e;
	}

	mlibc::panicLogger()
	    << "Unexpected zero return from sigsuspend()" << frg::endlog;

	return 0;
}

}