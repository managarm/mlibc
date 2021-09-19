#ifndef MLIBC_LINUX_SYSDEPS
#define MLIBC_LINUX_SYSDEPS

#include <sys/epoll.h>
#include <poll.h>
#include <abi-bits/pid_t.h>
#include <bits/ssize_t.h>
#include <bits/size_t.h>

namespace [[gnu::visibility("hidden")]] mlibc {

int sys_open(const char *pathname, int flags, int *fd);
int sys_close(int fd);
int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);

[[gnu::weak]] int sys_dup2(int fd, int flags, int newfd);
[[gnu::weak]] int sys_fork(pid_t *child);
[[gnu::weak]] int sys_inotify_create(int flags, int *fd);
[[gnu::weak]] int sys_inotify_add_watch(int ifd, const char *path, uint32_t mask, int *wd);
[[gnu::weak]] int sys_inotify_rm_watch(int ifd, int wd);
[[gnu::weak]] int sys_epoll_create(int flags, int *fd);
[[gnu::weak]] int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev);
[[gnu::weak]] int sys_epoll_pwait(int epfd, struct epoll_event *ev, int n,
		int timeout, const sigset_t *sigmask, int *raised);
[[gnu::weak]] int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events);
[[gnu::weak]] int sys_mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data);
[[gnu::weak]] int sys_umount2(const char *target, int flags);
[[gnu::weak]] int sys_eventfd_create(unsigned int initval, int flags, int *fd);
[[gnu::weak]] int sys_timerfd_create(int flags, int *fd);
[[gnu::weak]] int sys_timerfd_settime(int fd, int flags,
		const struct itimerspec *value);
[[gnu::weak]] int sys_signalfd_create(sigset_t, int flags, int *fd);
[[gnu::weak]] int sys_reboot(int cmd);

} // namespace mlibc

#endif // MLIBX_LINUX_SYSDEPS
