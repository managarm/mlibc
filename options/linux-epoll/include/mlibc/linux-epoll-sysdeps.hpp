#ifndef MLIBC_LINUX_EPOLL_SYSDEPS
#define MLIBC_LINUX_EPOLL_SYSDEPS

#include <sys/epoll.h>

namespace [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_epoll_create(int flags, int *fd);
[[gnu::weak]] int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev);
[[gnu::weak]] int sys_epoll_pwait(
    int epfd, struct epoll_event *ev, int n, int timeout, const sigset_t *sigmask, int *raised
);

} // namespace mlibc

#endif // MLIBC_LINUX_EPOLL_SYSDEPS
