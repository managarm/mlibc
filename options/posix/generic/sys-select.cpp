
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc-config.h>

#include <mlibc/posix-sysdeps.hpp>

void __FD_CLR(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	set->fds_bits[fd / 8] &= ~(1 << (fd % 8));
}
int __FD_ISSET(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	return set->fds_bits[fd / 8] & (1 << (fd % 8));
}
void __FD_SET(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	set->fds_bits[fd / 8] |= 1 << (fd % 8);
}
void __FD_ZERO(fd_set *set) {
	memset(set->fds_bits, 0, sizeof(fd_set));
}

int select(int num_fds, fd_set *__restrict read_set, fd_set *__restrict write_set,
		fd_set *__restrict except_set, struct timeval *__restrict timeout) {
	int num_events = 0;
	struct timespec timeouts = {};
	struct timespec *timeout_ptr = NULL;
	if (timeout) {
		timeouts.tv_sec = timeout->tv_sec;
		timeouts.tv_nsec = timeout->tv_usec * 1000;
		timeout_ptr = &timeouts;
	}

    MLIBC_CHECK_OR_ENOSYS(mlibc::sys_pselect, -1);
	if(int e = mlibc::sys_pselect(num_fds, read_set, write_set, except_set,
				timeout_ptr, NULL, &num_events); e) {
		errno = e;
		return -1;
	}
	return num_events;
}

int pselect(int num_fds, fd_set *__restrict read_set, fd_set *__restrict write_set,
		fd_set *__restrict except_set, const struct timespec *timeout, const sigset_t *sigmask) {
	int num_events = 0;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_pselect, -1);
	if(int e = mlibc::sys_pselect(num_fds, read_set, write_set, except_set,
				timeout, sigmask, &num_events); e) {
		errno = e;
		return -1;
	}
	return num_events;
}
