#ifndef _SYS_EPOLL_H
#define _SYS_EPOLL_H

#include <abi-bits/epoll.h>
#include <abi-bits/signal.h>

#ifndef __MLIBC_ABI_ONLY

#ifdef __cplusplus
extern "C" {
#endif

int epoll_create(int __flags);
int epoll_create1(int __flags);
int epoll_ctl(int __epfd, int __mode, int __fd, struct epoll_event *__ev);
int epoll_wait(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout);
int epoll_pwait(
    int __epfd,
    struct epoll_event *__events,
    int __maxevents,
    int __timeout,
    const sigset_t *__sigmask
);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_EPOLL_H */
