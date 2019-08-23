#ifndef MLIBC_SYSDEPS
#define MLIBC_SYSDEPS

#include <stddef.h>

#include <abi-bits/seek-whence.h>
#include <abi-bits/vm-flags.h>
#include <bits/posix/off_t.h>
#include <bits/posix/ssize_t.h>

#ifndef MLIBC_BUILDING_RTDL
#	include <fcntl.h>
#	include <time.h>
#	include <bits/posix/pid_t.h>
#	include <bits/posix/socklen_t.h>
#	include <bits/posix/stat.h>
#	include <poll.h>
#	include <stdarg.h>
#	include <sys/epoll.h>
#	include <sys/socket.h>
#	include <sys/resource.h>
#	include <sys/select.h>
#	include <termios.h>
#	include <time.h>
#endif

namespace mlibc [[gnu::visibility("hidden")]] {

enum class fsfd_target {
	none,
	path,
	fd,
	fd_path
};

void sys_libc_log(const char *message);
[[noreturn]] void sys_libc_panic();

int sys_futex_wait(int *pointer, int expected);
int sys_futex_wake(int *pointer);

int sys_tcb_set(void *pointer);

int sys_anon_allocate(size_t size, void **pointer);
int sys_anon_free(void *pointer, size_t size);

#ifndef MLIBC_BUILDING_RTDL
	[[noreturn]] void sys_exit(int status);
	int sys_clock_get(int clock, time_t *secs, long *nanos);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_open(const char *pathname, int flags, int *fd);

#ifndef MLIBC_BUILDING_RTDL
	[[gnu::weak]] int sys_open_dir(const char *path, int *handle);
	[[gnu::weak]] int sys_read_entries(int handle, void *buffer, size_t max_size,
			size_t *bytes_read);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);

#ifndef MLIBC_BUILDING_RTDL
	int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset);
int sys_close(int fd);

#ifndef MLIBC_BUILDING_RTDL
	[[gnu::weak]] int sys_access(const char *path, int mode);
	[[gnu::weak]] int sys_dup(int fd, int flags, int *newfd);
	[[gnu::weak]] int sys_dup2(int fd, int flags, int newfd);
	// In contrast to the isatty() library function, the sysdep function uses return value
	// zero (and not one) to indicate that the file is a terminal.
	[[gnu::weak]] int sys_isatty(int fd);
	[[gnu::weak]] int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags,
			struct stat *statbuf);
	[[gnu::weak]] int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length);
	[[gnu::weak]] int sys_ftruncate(int fd, size_t size);
	[[gnu::weak]] int sys_fallocate(int fd, off_t offset, size_t size);
	[[gnu::weak]] int sys_unlink(const char *path);
	[[gnu::weak]] int sys_socket(int family, int type, int protocol, int *fd);
	[[gnu::weak]] int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length);
	[[gnu::weak]] int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length);
	[[gnu::weak]] gid_t sys_getgid();
	[[gnu::weak]] gid_t sys_getegid();
	[[gnu::weak]] uid_t sys_getuid();
	[[gnu::weak]] uid_t sys_geteuid();
	[[gnu::weak]] pid_t sys_getpid();
	[[gnu::weak]] pid_t sys_getppid();
	[[gnu::weak]] pid_t sys_getpgrp();
	[[gnu::weak]] void sys_yield();
	[[gnu::weak]] int sys_sleep(time_t *secs, long *nanos);
	[[gnu::weak]] int sys_fork(pid_t *child);
	[[gnu::weak]] int sys_execve(const char *path, char *const argv[], char *const envp[]);
	[[gnu::weak]] int sys_select(int num_fds, fd_set *read_set, fd_set *write_set,
			fd_set *except_set, struct timeval *timeout);
	[[gnu::weak]] int sys_getrusage(int scope, struct rusage *usage);
	[[gnu::weak]] int sys_getrlimit(int resource, struct rlimit *limit);
	[[gnu::weak]] int sys_timerfd_create(int flags, int *fd);
	[[gnu::weak]] int sys_timerfd_settime(int fd, int flags,
			const struct itimerspec *value);
	[[gnu::weak]] int sys_signalfd_create(sigset_t, int flags, int *fd);
	[[gnu::weak]] int sys_getcwd(char *buffer, size_t size);
	[[gnu::weak]] int sys_chdir(const char *path);
	[[gnu::weak]] int sys_fchdir(int fd);
	[[gnu::weak]] int sys_chroot(const char *path);
	[[gnu::weak]] int sys_mkdir(const char *path);
	[[gnu::weak]] int sys_symlink(const char *target_path, const char *link_path);
	[[gnu::weak]] int sys_rename(const char *path, const char *new_path);
	[[gnu::weak]] int sys_fcntl(int fd, int request, va_list args, int *result);
	[[gnu::weak]] int sys_ttyname(int fd, char *buf, size_t size);
	[[gnu::weak]] int sys_fadvise(int fd, off_t offset, off_t length, int advice);
	[[gnu::weak]] int sys_fsync(int fd);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window);

#ifndef MLIBC_BUILDING_RTDL
	[[gnu::weak]] int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_vm_unmap(void *pointer, size_t size);

[[gnu::weak]] int sys_vm_readahead(void *pointer, size_t size);

#ifndef MLIBC_BUILDING_RTDL
	[[gnu::weak]] int sys_tcgetattr(int fd, struct termios *attr);
	[[gnu::weak]] int sys_tcsetattr(int, int, const struct termios *attr);
	[[gnu::weak]] int sys_tcflow(int, int);
	[[gnu::weak]] int sys_pipe(int *fds, int flags);
	[[gnu::weak]] int sys_socketpair(int domain, int type_and_flags, int proto, int *fds);
	[[gnu::weak]] int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events);
	[[gnu::weak]] int sys_epoll_create(int flags, int *fd);
	[[gnu::weak]] int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev);
	[[gnu::weak]] int sys_epoll_wait(int epfd, struct epoll_event *evnts, int n,
			int timeout, int *raised);
	[[gnu::weak]] int sys_inotify_create(int flags, int *fd);
	[[gnu::weak]] int sys_inotify_add_watch(int ifd, const char *path, uint32_t mask, int *wd);
	[[gnu::weak]] int sys_inotify_rm_watch(int ifd, int wd);
	[[gnu::weak]] int sys_ioctl(int fd, unsigned long request, void *arg, int *result);
	[[gnu::weak]] int sys_getsockopt(int fd, int layer, int number,
			void *__restrict buffer, socklen_t *__restrict size);
	[[gnu::weak]] int sys_setsockopt(int fd, int layer, int number,
			const void *buffer, socklen_t size);
	[[gnu::weak]] int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid);
	[[gnu::weak]] int sys_mount(const char *source, const char *target,
			const char *fstype, unsigned long flags, const void *data);
	[[gnu::weak]] int sys_sigprocmask(int how, const sigset_t *__restrict set,
			sigset_t *__restrict retrieve);
	[[gnu::weak]] int sys_sigaction(int, const struct sigaction *__restrict,
			struct sigaction *__restrict);
	[[gnu::weak]] int sys_kill(int, int);
	[[gnu::weak]] int sys_accept(int fd, int *newfd);
	[[gnu::weak]] int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
	[[gnu::weak]] int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
	[[gnu::weak]] int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
			socklen_t *actual_length);
	[[gnu::weak]] int sys_gethostname(char *buffer, size_t bufsize);
	[[gnu::weak]] int sys_eventfd_create(unsigned int initval, int flags, int *fd);
#endif // !defined(MLIBC_BUILDING_RTDL)

} //namespace mlibc

#endif // MLIBC_SYSDEPS
