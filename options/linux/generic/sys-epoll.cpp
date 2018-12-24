
#include <errno.h>
#include <sys/epoll.h>

#include <bits/ensure.h>
#include <mlibc/sysdeps.hpp>

int epoll_create(int) {
	int fd;
	if(int e = mlibc::sys_epoll_create(0, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int epoll_pwait(int, struct epoll_event *, int, int, const sigset_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int epoll_create1(int flags) {
	int fd;
	if(int e = mlibc::sys_epoll_create(flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev) {
	if(int e = mlibc::sys_epoll_ctl(epfd, mode, fd, ev); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int epoll_wait(int epfd, struct epoll_event *evnts, int n, int timeout) {
	int raised;
	if(int e = mlibc::sys_epoll_wait(epfd, evnts, n, timeout, &raised)) {
		errno = e;
		return -1;
	}
	return raised;
}

