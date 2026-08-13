#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>

namespace {

int syscall_error(long result) {
	return result < 0 ? static_cast<int>(-result) : 0;
}

} // namespace

namespace mlibc {

extern "C" void __mlibc_restorer();

int Sysdeps<Kill>::operator()(pid_t pid, int signal) {
	return syscall_error(roxy_syscall2(ROXY_SYS_SEND_SIGNAL, pid, signal));
}

int Sysdeps<Sigprocmask>::operator()(
	int how,
	const sigset_t *__restrict set,
	sigset_t *__restrict retrieve
) {
	return syscall_error(roxy_syscall3(
	    ROXY_SYS_SIGPROCMASK,
	    how,
	    reinterpret_cast<long>(set),
	    reinterpret_cast<long>(retrieve)
	));
}

int Sysdeps<Sigaction>::operator()(
	int signal,
	const struct sigaction *__restrict action,
	struct sigaction *__restrict old_action
) {
	if (action) {
		struct sigaction modified = *action;
		if (modified.sa_handler != SIG_DFL && modified.sa_handler != SIG_IGN)
			modified.sa_restorer = __mlibc_restorer;

		return syscall_error(roxy_syscall3(
		    ROXY_SYS_SIGACTION,
		    signal,
		    reinterpret_cast<long>(&modified),
		    reinterpret_cast<long>(old_action)
		));
	}

	return syscall_error(roxy_syscall3(
	    ROXY_SYS_SIGACTION,
	    signal,
	    0,
	    reinterpret_cast<long>(old_action)
	));
}

} // namespace mlibc
