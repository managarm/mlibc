#ifndef _ABIBITS_EPOLL_H
#define _ABIBITS_EPOLL_H

#include <mlibc-config.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/signal.h>
#include <stdint.h>

#if !__MLIBC_LINUX_EPOLL_OPTION
#  error "<sys/epoll.h> is tied to the linux-epoll option. Enable it or do not use this header."
#endif /* !__MLIBC_LINUX_EPOLL_OPTION */

#define EPOLL_NONBLOCK O_NONBLOCK
#define EPOLL_CLOEXEC 02000000 /* Same as __MLIBC_O_CLOEXEC */

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
#if defined(__x86_64__) || defined(__i386__)
__attribute__((__packed__))
#endif
;

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_EPOLL_H */
