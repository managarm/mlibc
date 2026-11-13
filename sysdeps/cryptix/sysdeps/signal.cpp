#include <cryptix/syscall.h>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc {
int Sysdeps<Sigaction>::operator()(
    int signum, const struct sigaction *__restrict act, struct sigaction *__restrict oldact
) {
	auto ret = Syscall(SYS_SIGACTION, signum, act, oldact);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Sigprocmask>::operator()(int how, const sigset_t *set, sigset_t *retrieve) {
	auto ret = Syscall(SYS_SIGPROCMASK, how, set, retrieve, NSIG / 8);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Sigpending>::operator()(sigset_t *set) {
	auto ret = Syscall(SYS_SIGPENDING, set);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<Sigtimedwait>::operator()(
    const sigset_t *set, siginfo_t *info, const struct timespec *timeout, int *out_signal
) {
	auto ret = Syscall(SYS_SIGTIMEDWAIT, set, info, timeout);
	if (auto e = syscall_error(ret); e)
		return e;

	*out_signal = ret;
	return 0;
}
int Sysdeps<Sigsuspend>::operator()(const sigset_t *set) {
	auto ret = Syscall(SYS_SIGSUSPEND, set);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Sigaltstack>::operator()(const stack_t *ss, stack_t *oss) {
	auto ret = Syscall(SYS_SIGALTSTACK, ss, oss);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
} // namespace mlibc
