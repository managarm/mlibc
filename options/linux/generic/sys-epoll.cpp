
#include <errno.h>
#include <sys/epoll.h>

#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>
#include <stddef.h>

int epoll_create(int) {
	int fd;
	if(!mlibc::sys_epoll_create) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_epoll_create(0, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int epoll_pwait(int epfd, struct epoll_event *evnts, int n, int timeout,
		const sigset_t *sigmask) {
	int raised;
	if(!mlibc::sys_epoll_pwait) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_epoll_pwait(epfd, evnts, n, timeout, sigmask, &raised)) {
		errno = e;
		return -1;
	}
	return raised;
}

int epoll_create1(int flags) {
	int fd;
	if(!mlibc::sys_epoll_create) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_epoll_create(flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev) {
	if(!mlibc::sys_epoll_ctl) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_epoll_ctl(epfd, mode, fd, ev); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int epoll_wait(int epfd, struct epoll_event *evnts, int n, int timeout) {
	int raised;
	if(!mlibc::sys_epoll_pwait) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_epoll_pwait(epfd, evnts, n, timeout, NULL, &raised)) {
		errno = e;
		return -1;
	}
	return raised;
}

