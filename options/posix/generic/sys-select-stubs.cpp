
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc-config.h>

#include <mlibc/posix-sysdeps.hpp>

void FD_CLR(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	set->__mlibc_elems[fd / 8] &= ~(1 << (fd % 8));
}
int FD_ISSET(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	return set->__mlibc_elems[fd / 8] & (1 << (fd % 8));
}
void FD_SET(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	set->__mlibc_elems[fd / 8] |= 1 << (fd % 8);
}
void FD_ZERO(fd_set *set) {
	memset(set->__mlibc_elems, 0, sizeof(fd_set));
}

int select(int num_fds, fd_set *__restrict read_set, fd_set *__restrict write_set,
		fd_set *__restrict except_set, struct timeval *__restrict timeout) {
    if(!mlibc::sys_pselect) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	int num_events = 0;
	struct timespec timeouts = {};
	struct timespec *timeout_ptr = NULL;
	if (timeout) {
		timeouts.tv_sec = timeout->tv_sec;
		timeouts.tv_nsec = timeout->tv_usec * 1000;
		timeout_ptr = &timeouts;
	}

	if(int e = mlibc::sys_pselect(num_fds, read_set, write_set, except_set,
				timeout_ptr, NULL, &num_events); e) {
		errno = e;
		return -1;
	}
	return num_events;
}

int pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set,
		const struct timespec *timeout,	const sigset_t *sigmask) {
	if(!mlibc::sys_pselect) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	int num_events = 0;
	if(int e = mlibc::sys_pselect(num_fds, read_set, write_set, except_set,
				timeout, sigmask, &num_events); e) {
		errno = e;
		return -1;
	}
	return num_events;
}
