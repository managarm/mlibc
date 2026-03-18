
#include <errno.h>
#include <poll.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int poll(struct pollfd *fds, nfds_t count, int timeout) {
	int num_events;
	if(int e = mlibc::sysdep_or_enosys<Poll>(fds, count, timeout, &num_events); e) {
		errno = e;
		return -1;
	}
	return num_events;
}

int ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *timeout_ts, const sigset_t *sigmask) {
	if constexpr (mlibc::IsImplemented<Ppoll>) {
		int num_events;
		if(int e = mlibc::sysdep_or_panic<Ppoll>(fds, nfds, timeout_ts, sigmask, &num_events); e) {
			errno = e;
			return -1;
		}
		return num_events;
	}

	sigset_t origmask;
	int timeout = (timeout_ts == nullptr) ? -1 : (timeout_ts->tv_sec * 1000 + timeout_ts->tv_nsec / 1000000);

	sigprocmask(SIG_SETMASK, sigmask, &origmask);
	int ready = poll(fds, nfds, timeout);
	sigprocmask(SIG_SETMASK, &origmask, nullptr);

	return ready;
}
