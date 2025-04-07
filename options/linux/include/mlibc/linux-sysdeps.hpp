#ifndef MLIBC_LINUX_SYSDEPS
#define MLIBC_LINUX_SYSDEPS

#include <ifaddrs.h>
#include <sched.h>
#include <stdarg.h>
#include <sys/epoll.h>
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#include <poll.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/statx.h>
#include <bits/off_t.h>
#include <bits/ssize_t.h>
#include <bits/size_t.h>

namespace [[gnu::visibility("hidden")]] mlibc {

int sys_open(const char *pathname, int flags, mode_t mode, int *fd);
int sys_close(int fd);
int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);
int sys_ioctl(int fd, unsigned long request, void *arg, int *result);

[[gnu::weak]] int sys_dup2(int fd, int flags, int newfd);
[[gnu::weak]] int sys_fork(pid_t *child);
[[gnu::weak]] int sys_inotify_create(int flags, int *fd);
[[gnu::weak]] int sys_inotify_add_watch(int ifd, const char *path, uint32_t mask, int *wd);
[[gnu::weak]] int sys_inotify_rm_watch(int ifd, int wd);
[[gnu::weak]] int sys_epoll_create(int flags, int *fd);
[[gnu::weak]] int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev);
[[gnu::weak]] int sys_epoll_pwait(int epfd, struct epoll_event *ev, int n,
		int timeout, const sigset_t *sigmask, int *raised);
[[gnu::weak]] int sys_mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data);
[[gnu::weak]] int sys_umount2(const char *target, int flags);
[[gnu::weak]] int sys_eventfd_create(unsigned int initval, int flags, int *fd);
[[gnu::weak]] int sys_timerfd_create(int clockid, int flags, int *fd);
[[gnu::weak]] int sys_timerfd_settime(int fd, int flags,
		const struct itimerspec *value, struct itimerspec *oldvalue);
[[gnu::weak]] int sys_timerfd_gettime(int fd, struct itimerspec *its);
[[gnu::weak]] int sys_signalfd_create(const sigset_t *, int flags, int *fd);
[[gnu::weak]] int sys_reboot(int cmd);
[[gnu::weak]] int sys_ptrace(long req, pid_t pid, void *addr, void *data, long *out);
[[gnu::weak]] int sys_prctl(int option, va_list va, int *out);
[[gnu::weak]] int sys_init_module(void *module, unsigned long length, const char *args);
[[gnu::weak]] int sys_delete_module(const char *name, unsigned flags);
[[gnu::weak]] int sys_klogctl(int type, char *bufp, int len, int *out);
[[gnu::weak]] int sys_getcpu(int *cpu);

[[gnu::weak]] int sys_sysinfo(struct sysinfo *info);
[[gnu::weak]] int sys_swapon(const char *path, int flags);
[[gnu::weak]] int sys_swapoff(const char *path);

[[gnu::weak]] int sys_setxattr(const char *path, const char *name,
	const void *val, size_t size, int flags);
[[gnu::weak]] int sys_lsetxattr(const char *path, const char *name,
	const void *val, size_t size, int flags);
[[gnu::weak]] int sys_fsetxattr(int fd, const char *name, const void *val,
	size_t size, int flags);

[[gnu::weak]] int sys_getxattr(const char *path, const char *name,
	void *val, size_t size, ssize_t *nread);
[[gnu::weak]] int sys_lgetxattr(const char *path, const char *name,
	void *val, size_t size, ssize_t *nread);
[[gnu::weak]] int sys_fgetxattr(int fd, const char *name, void *val,
	size_t size, ssize_t *nread);

[[gnu::weak]] int sys_listxattr(const char *path, char *list, size_t size,
	ssize_t *nread);
[[gnu::weak]] int sys_llistxattr(const char *path, char *list, size_t size,
	ssize_t *nread);
[[gnu::weak]] int sys_flistxattr(int fd, char *list, size_t size,
	ssize_t *nread);

[[gnu::weak]] int sys_removexattr(const char *path, const char *name);
[[gnu::weak]] int sys_lremovexattr(const char *path, const char *name);
[[gnu::weak]] int sys_fremovexattr(int fd, const char *name);

[[gnu::weak]] int sys_statfs(const char *path, struct statfs *buf);
[[gnu::weak]] int sys_fstatfs(int fd, struct statfs *buf);

[[gnu::weak]] int sys_statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf);

[[gnu::weak]] int sys_getifaddrs(struct ifaddrs **);

[[gnu::weak]] int sys_sendfile(int outfd, int infd, off_t *offset, size_t count, ssize_t *out);
[[gnu::weak]] int sys_syncfs(int fd);
[[gnu::weak]] int sys_unshare(int flags);
[[gnu::weak]] int sys_setns(int fd, int nstype);

[[gnu::weak]] int sys_pidfd_open(pid_t pid, unsigned int flags, int *outfd);
[[gnu::weak]] int sys_pidfd_getpid(int fd, pid_t *outpid);
[[gnu::weak]] int sys_pidfd_send_signal(int pidfd, int sig, siginfo_t *info, unsigned int flags);

} // namespace mlibc

#endif // MLIBX_LINUX_SYSDEPS
