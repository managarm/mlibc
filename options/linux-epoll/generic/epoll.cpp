#include <errno.h>
#include <sys/epoll.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <stddef.h>

int epoll_create(int) {
	int fd;
	if (int e = mlibc::sysdep_or_enosys<EpollCreate>(0, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int epoll_pwait(int epfd, struct epoll_event *evnts, int n, int timeout, const sigset_t *sigmask) {
	int raised;
	if (int e = mlibc::sysdep_or_enosys<EpollPwait>(epfd, evnts, n, timeout, sigmask, &raised)) {
		errno = e;
		return -1;
	}
	return raised;
}

int epoll_create1(int flags) {
	int fd;
	if (int e = mlibc::sysdep_or_enosys<EpollCreate>(flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev) {
	if (int e = mlibc::sysdep_or_enosys<EpollCtl>(epfd, mode, fd, ev); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int epoll_wait(int epfd, struct epoll_event *evnts, int n, int timeout) {
	int raised;
	if (int e = mlibc::sysdep_or_enosys<EpollPwait>(epfd, evnts, n, timeout, nullptr, &raised)) {
		errno = e;
		return -1;
	}
	return raised;
}
