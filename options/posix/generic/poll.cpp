
#include <errno.h>
#include <poll.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int poll(struct pollfd *fds, nfds_t count, int timeout) {
	int num_events;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_poll, -1);
	if(int e = mlibc::sys_poll(fds, count, timeout, &num_events); e) {
		errno = e;
		return -1;
	}
	return num_events;
}

#if __MLIBC_LINUX_OPTION
int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask) {
	sigset_t origmask;
	int timeout = (timeout_ts == NULL) ? -1 : (timeout_ts->tv_sec * 1000 + timeout_ts->tv_nsec / 1000000);

	sigprocmask(SIG_SETMASK, sigmask, &origmask);
	int ready = poll(fds, nfds, timeout);
	sigprocmask(SIG_SETMASK, &origmask, NULL);

	return ready;
}
#endif // __MLIBC_LINUX_OPTION

