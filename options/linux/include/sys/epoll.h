#ifndef	_SYS_EPOLL_H
#define	_SYS_EPOLL_H

#include <stdint.h>
#include <abi-bits/signal.h>
#include <abi-bits/epoll.h>
#include <abi-bits/fcntl.h>

#define EPOLL_NONBLOCK O_NONBLOCK

/* These constants match the Linux definitions. */
#define EPOLLIN 0x001
#define EPOLLPRI 0x002
#define EPOLLOUT 0x004
#define EPOLLRDNORM 0x040
#define EPOLLRDBAND 0x080
#define EPOLLWRNORM 0x100
#define EPOLLWRBAND 0x200
#define EPOLLMSG 0x400
#define EPOLLERR 0x008
#define EPOLLHUP 0x010
#define EPOLLRDHUP 0x2000
#define EPOLLEXCLUSIVE (1U << 28)
#define EPOLLWAKEUP (1U << 29)
#define EPOLLONESHOT (1U << 30)
#define EPOLLET (1U << 31)

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

#ifdef __cplusplus
extern "C" {
#endif

typedef union epoll_data {
	void *ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;

struct epoll_event {
	uint32_t events;
	epoll_data_t data;
}
#ifdef __x86_64__
__attribute__((__packed__))
#endif
;

#ifndef __MLIBC_ABI_ONLY

int epoll_create(int __flags);
int epoll_create1(int __flags);
int epoll_ctl(int __epfd, int __mode, int __fd, struct epoll_event *__ev);
int epoll_wait(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout);
int epoll_pwait(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout, const sigset_t *__sigmask);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_EPOLL_H */
