
#include <string.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <unistd.h>

#include <frigg/debug.hpp>
#include <bits/ensure.h>

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
	__ensure(!timeout);

	// TODO: Do not keep errors from epoll (?).
	int fd = epoll_create1(0);
	if(fd == -1)
		return -1;

	for(int k = 0; k < FD_SETSIZE; k++) {
		struct epoll_event ev;
		memset(&ev, 0, sizeof(struct epoll_event));

		if(read_set && FD_ISSET(k, read_set))
			ev.events |= EPOLLIN; // TODO: Additional events.
		if(write_set && FD_ISSET(k, write_set))
			ev.events |= EPOLLOUT; // TODO: Additional events.
		if(except_set && FD_ISSET(k, except_set))
			ev.events |= EPOLLPRI;

		if(!ev.events)
			continue;
		ev.data.u32 = k;
		
		if(epoll_ctl(fd, EPOLL_CTL_ADD, k, &ev))
			return -1;
	}

	struct epoll_event evnts[16];
	int n = epoll_wait(fd, evnts, 16, -1);
	if(n == -1)
		return -1;
	
	fd_set res_read_set;
	fd_set res_write_set;
	fd_set res_except_set;
	FD_ZERO(&res_read_set);
	FD_ZERO(&res_write_set);
	FD_ZERO(&res_except_set);
	int m = 0;

	for(int i = 0; i < n; i++) {
		int k = evnts[i].data.u32;

		if(read_set && FD_ISSET(k, read_set)
				&& evnts[i].events & EPOLLIN) {
			FD_SET(k, &res_read_set);
			m++;
		}

		if(write_set && FD_ISSET(k, write_set)
				&& evnts[i].events & EPOLLOUT) {
			FD_SET(k, &res_write_set);
			m++;
		}

		if(except_set && FD_ISSET(k, except_set)
				&& evnts[i].events & EPOLLPRI) {
			FD_SET(k, &res_except_set);
			m++;
		}
	}

	if(close(fd))
		__ensure("close() failed on epoll file");
	
	if(read_set)
		memcpy(read_set, &res_read_set, sizeof(fd_set));
	if(write_set)
		memcpy(write_set, &res_write_set, sizeof(fd_set));
	if(except_set)
		memcpy(except_set, &res_except_set, sizeof(fd_set));

	return m;
}

