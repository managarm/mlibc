#include <asm/ioctls.h>

#include <keyronex/syscall.h>

#include <sys/epoll.h>
#include <sys/poll.h>
#include <sys/select.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <bits/ensure.h>
#include <frg/logging.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {

int
sys_epoll_pwait(int epfd, struct epoll_event *ev, int n, int timeout,
    const sigset_t *sigmask, int *raised)
{
	(void)sigmask;
	int r = syscall4(SYS_epoll_wait, epfd, (uintptr_t)ev, n, timeout,
	    NULL);
	if (r < 0)
		return -r;
	*raised = r;
	return 0;
}

int
sys_epoll_create(int flags, int *fd)
{
	int r = syscall1(SYS_epoll_create, flags, NULL);
	if (r < 0)
		return -r;
	*fd = r;
	return 0;
}

int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev)
{
	int r = syscall4(SYS_epoll_ctl, epfd, mode, fd, (uintptr_t)ev,
	    NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_ppoll(struct pollfd *fds, nfds_t nfds, const struct timespec *ts,
    const sigset_t *sigmask, int *num_events)
{
	int epfd, ready;
	struct epoll_event ev, events[nfds];
	int timeout_ms;
	int err;

	epfd = epoll_create1(0);
	if (epfd == -1) {
		err = errno;
		perror("sys_ppoll(epoll_create1)");
		return err;
	}

	for (nfds_t i = 0; i < nfds; i++) {
		ev.events = 0;
		if (fds[i].events & POLLIN)
			ev.events |= EPOLLIN;
		if (fds[i].events & POLLOUT)
			ev.events |= EPOLLOUT;
		if (fds[i].events & POLLPRI)
			ev.events |= EPOLLPRI;
		ev.data.fd = i;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, fds[i].fd, &ev) == -1) {
			err = errno;
			perror("sys_ppoll(epoll_ctl)");
			close(epfd);
			return err;
		}
	}

	if (ts)
		timeout_ms = ts->tv_sec * 1000 + ts->tv_nsec / 1000000;
	else
		timeout_ms = -1;

	ready = epoll_pwait(epfd, events, nfds, timeout_ms, sigmask);
	if (ready == -1) {
		err = errno;
		perror("sys_ppoll(epoll_pwait)");
		close(epfd);
		return err;
	}

	for (nfds_t i = 0; i < nfds; i++)
		fds[i].revents = 0;

	for (int i = 0; i < ready; i++) {
		int idx = events[i].data.fd;
		fds[idx].revents = 0;
		if (events[i].events & EPOLLIN)
			fds[idx].revents |= POLLIN;
		if (events[i].events & EPOLLOUT)
			fds[idx].revents |= POLLOUT;
		if (events[i].events & EPOLLPRI)
			fds[idx].revents |= POLLPRI;
		if (events[i].events & EPOLLERR)
			fds[idx].revents |= POLLERR;
		if (events[i].events & EPOLLHUP)
			fds[idx].revents |= POLLHUP;
	}

	close(epfd);

	*num_events = ready;

	return 0;
}

int
sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events)
{
	struct timespec ts;
	ts.tv_sec = timeout / 1000;
	ts.tv_nsec = (timeout % 1000) * 1000000;
	return sys_ppoll(fds, count, &ts, NULL, num_events);
}

int
sys_pselect(int, fd_set *read_set, fd_set *write_set,
    fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask,
    int *num_events)
{
	int fd = epoll_create1(0);

	if (fd == -1)
		return -1;

	for (int k = 0; k < FD_SETSIZE; k++) {
		struct epoll_event ev;
		memset(&ev, 0, sizeof(struct epoll_event));

		if (read_set && FD_ISSET(k, read_set))
			ev.events |= EPOLLIN;
		if (write_set && FD_ISSET(k, write_set))
			ev.events |= EPOLLOUT;
		if (except_set && FD_ISSET(k, except_set))
			ev.events |= EPOLLPRI;

		if (!ev.events)
			continue;

		ev.data.fd = k;
		if (epoll_ctl(fd, EPOLL_CTL_ADD, k, &ev))
			return -1;
	}

	struct epoll_event evnts[16];
	int n = epoll_pwait(fd, evnts, 16,
	    timeout ? (timeout->tv_sec * 1000 + timeout->tv_nsec / 100) : -1,
	    sigmask);

	if (n == -1)
		return -1;

	fd_set res_read_set;
	fd_set res_write_set;
	fd_set res_except_set;
	FD_ZERO(&res_read_set);
	FD_ZERO(&res_write_set);
	FD_ZERO(&res_except_set);

	int m = 0;

	for (int i = 0; i < n; i++) {
		int k = evnts[i].data.fd;

		if (read_set && FD_ISSET(k, read_set) &&
		    evnts[i].events & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
			FD_SET(k, &res_read_set);
			m++;
		}

		if (write_set && FD_ISSET(k, write_set) &&
		    evnts[i].events & (EPOLLOUT | EPOLLERR | EPOLLHUP)) {
			FD_SET(k, &res_write_set);
			m++;
		}

		if (except_set && FD_ISSET(k, except_set) &&
		    evnts[i].events & EPOLLPRI) {
			FD_SET(k, &res_except_set);
			m++;
		}
	}

	if (close(fd))
		__ensure("mlibc::pselect: close() failed on epoll file");

	if (read_set)
		memcpy(read_set, &res_read_set, sizeof(fd_set));

	if (write_set)
		memcpy(write_set, &res_write_set, sizeof(fd_set));

	if (except_set)
		memcpy(except_set, &res_except_set, sizeof(fd_set));

	*num_events = m;
	return 0;
}

} /* namespace mlibc */
