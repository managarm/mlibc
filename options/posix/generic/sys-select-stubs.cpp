
#include <string.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>

#include <mlibc/debug.hpp>
#include <bits/ensure.h>
#include <bits/feature.h>

#include <mlibc/sysdeps.hpp>

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

// select() is currently implemented on top of epoll.
// TODO: Provide a sys_select() function instead.
int select(int num_fds, fd_set *__restrict read_set, fd_set *__restrict write_set,
		fd_set *__restrict except_set, struct timeval *__restrict timeout) {
    if(!mlibc::sys_select) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	int num_events = 0;
	if(int e = mlibc::sys_select(num_fds, read_set, write_set, except_set,
				timeout, &num_events); e) {
		errno = e;
		return -1;
	}
	return num_events;
}
