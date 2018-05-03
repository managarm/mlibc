
#include <bits/ensure.h>
#include <poll.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

int poll(struct pollfd *fds, nfds_t count, int timeout) {
	int num_events;
	if(mlibc::sys_poll(fds, count, timeout, &num_events))
		return -1;
	return num_events;
}

