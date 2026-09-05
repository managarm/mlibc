#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>
#include <stdint.h>

namespace mlibc {

namespace {
int syscall_error(long result) {
	return result < 0 ? static_cast<int>(-result) : 0;
}
} // namespace

int Sysdeps<TimerCreate>::operator()(clockid_t clk, struct sigevent *evp, timer_t *res) {
	// Roxy supports only process-directed notifications (SIGEV_NONE and SIGEV_SIGNAL). The
	// thread-directed modes require per-thread signal delivery that Roxy does not provide yet;
	// reject them here instead of round-tripping an unsupported notification through the kernel.
	if(evp) {
		switch(evp->sigev_notify) {
			case SIGEV_NONE:
			case SIGEV_SIGNAL:
				break;
			default:
				return EINVAL;
		}
	}

	int32_t timer_id = 0;
	auto error = syscall_error(roxy_syscall3(
	    ROXY_SYS_TIMER_CREATE,
	    clk,
	    reinterpret_cast<long>(evp),
	    reinterpret_cast<long>(&timer_id)
	));
	if(error)
		return error;

	*res = reinterpret_cast<timer_t>(static_cast<intptr_t>(timer_id));
	return 0;
}

int Sysdeps<TimerSettime>::operator()(
    timer_t t, int flags, const struct itimerspec *val, struct itimerspec *old) {
	return syscall_error(roxy_syscall4(
	    ROXY_SYS_TIMER_SETTIME,
	    reinterpret_cast<long>(t),
	    flags,
	    reinterpret_cast<long>(val),
	    reinterpret_cast<long>(old)
	));
}

int Sysdeps<TimerGettime>::operator()(timer_t t, struct itimerspec *val) {
	return syscall_error(roxy_syscall2(
	    ROXY_SYS_TIMER_GETTIME,
	    reinterpret_cast<long>(t),
	    reinterpret_cast<long>(val)
	));
}

int Sysdeps<TimerGetoverrun>::operator()(timer_t t, int *out) {
	return syscall_error(roxy_syscall2(
	    ROXY_SYS_TIMER_GETOVERRUN,
	    reinterpret_cast<long>(t),
	    reinterpret_cast<long>(out)
	));
}

int Sysdeps<TimerDelete>::operator()(timer_t t) {
	return syscall_error(roxy_syscall1(ROXY_SYS_TIMER_DELETE, reinterpret_cast<long>(t)));
}

} // namespace mlibc