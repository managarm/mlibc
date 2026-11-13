#include <cryptix/syscall.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc {
int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
	timespec duration = {.tv_sec = *secs, .tv_nsec = nanos ? *nanos : 0};
	timespec remaining;
	auto ret = Syscall(SYS_NANOSLEEP, &duration, &remaining);
	if (auto e = syscall_error(ret); e) {
		if (e != EINTR)
			return e;
		*secs = remaining.tv_sec;
		if (nanos)
			*nanos = remaining.tv_nsec;
		return e;
	}

	return 0;
}
int Sysdeps<GetItimer>::operator()(int which, struct itimerval *curr_value) {
	auto ret = Syscall(SYS_GETITIMER, which, curr_value);
	if (auto e = syscall_error(ret); e)
		return e;
	return 0;
}
int Sysdeps<SetItimer>::operator()(
    int which, const struct itimerval *new_value, struct itimerval *old_value
) {
	auto ret = Syscall(SYS_SETITIMER, which, new_value, old_value);
	if (auto e = syscall_error(ret); e)
		return e;
	return 0;
}
int Sysdeps<ClockGet>::operator()(int clockid, time_t *secs, long *nanos) {
	timespec ts;
	auto ret = Syscall(SYS_CLOCK_GETTIME, clockid, &ts);
	if (auto e = syscall_error(ret); e)
		return e;

	*secs = ts.tv_sec;
	*nanos = ts.tv_nsec;
	return 0;
}
} // namespace mlibc
