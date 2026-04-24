#include <mlibc/all-sysdeps.hpp>
#include <sys/epoll.h>
#include <zinnia/syscall.hpp>

namespace mlibc {

int Sysdeps<TimerfdCreate>::operator()(int clockid, int flags, int *fd) {
	auto r = zinnia_syscall(SYSCALL_TIMERFD_CREATE, clockid, flags);
	if (r.error)
		return r.error;
	*fd = r.value;
	return 0;
}

int Sysdeps<TimerfdSettime>::operator()(
    int fd, int flags, const struct itimerspec *value, struct itimerspec *oldvalue
) {
	return zinnia_syscall(SYSCALL_TIMERFD_SETTIME, fd, flags, (size_t)value, (size_t)oldvalue)
	    .error;
}

int Sysdeps<TimerfdGettime>::operator()(int fd, struct itimerspec *its) {
	return zinnia_syscall(SYSCALL_TIMERFD_GETTIME, fd, (size_t)its).error;
}

} // namespace mlibc
