
#include <bits/ensure.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <frigg/debug.hpp>

int poll(struct pollfd *fds, nfds_t count, int timeout) {
	// TODO: Do not keep errors from epoll (?).
	int epfd = epoll_create1(0);
	if(epfd == -1)
		return -1;

	for(nfds_t k = 0; k < count; k++) {
		struct epoll_event ev;
		memset(&ev, 0, sizeof(struct epoll_event));

		if(fds[k].events & ~(POLLIN | POLLOUT))
			frigg::infoLogger() << "\e[31mmlibc: Unexpected events for poll()\e[39m"
					<< frigg::endLog;
		if(fds[k].events & POLLIN)
			ev.events |= EPOLLIN;
		if(fds[k].events & POLLOUT)
			ev.events |= EPOLLOUT;
		
		fds[k].revents = 0;
		
		if(!ev.events)
			continue;
		ev.data.u32 = k;
		
		if(epoll_ctl(epfd, EPOLL_CTL_ADD, fds[k].fd, &ev))
			return -1;
	}

	struct epoll_event evnts[16];
	int n = epoll_wait(epfd, evnts, 16, timeout);
	if(n == -1)
		return -1;

	int m = 0;
	for(int i = 0; i < n; i++) {
		int k = evnts[i].data.u32;

		if((fds[k].events & POLLIN) && (evnts[i].events & EPOLLIN)) {
			fds[i].revents |= POLLIN;
			m++;
		}

		if((fds[k].events & POLLIN) && (evnts[i].events & EPOLLOUT)) {
			fds[i].revents |= POLLOUT;
			m++;
		}
	}
	
	if(close(epfd))
		__ensure("close() failed on epoll file");

	return m;
}

