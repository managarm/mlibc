
#include <stddef.h>

#include <bits/ansi/seek.h>
#include <bits/posix/map_flags.h>
#include <bits/posix/off_t.h>
#include <bits/posix/ssize_t.h>

#ifndef MLIBC_BUILDING_RTDL
#	include <time.h>
#	include <bits/posix/pid_t.h>
#	include <bits/posix/socklen_t.h>
#	include <bits/posix/stat.h>
#	include <poll.h>
#	include <stdarg.h>
#	include <sys/epoll.h>
#	include <sys/socket.h>
#	include <termios.h>
#	include <time.h>
#endif

namespace mlibc [[gnu::visibility("hidden")]] {

void sys_libc_log(const char *message);
__attribute__ ((noreturn)) void sys_libc_panic();

int sys_futex_wait(int *pointer, int expected);
int sys_futex_wake(int *pointer);

int sys_tcb_set(void *pointer);

int sys_anon_allocate(size_t size, void **pointer);
int sys_anon_free(void *pointer, size_t size);

#ifndef MLIBC_BUILDING_RTDL
	__attribute__ ((noreturn)) void sys_exit(int status);
	int sys_clock_get(int clock, time_t *secs, long *nanos);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_open(const char *pathname, int flags, int *fd);

#ifndef MLIBC_BUILDING_RTDL
	int sys_open_dir(const char *path, int *handle);
	int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);

#ifndef MLIBC_BUILDING_RTDL
	int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset);
int sys_close(int fd);

#ifndef MLIBC_BUILDING_RTDL
	int sys_access(const char *path, int mode);
	int sys_dup(int fd, int flags, int *newfd);
	int sys_dup2(int fd, int flags, int newfd);
	// In contrast to the isatty() library function, the sysdep function uses return value
	// zero (and not one) to indicate that the file is a terminal.
	int sys_isatty(int fd);
	int sys_stat(const char *path, struct stat *statbuf);
	int sys_lstat(const char *path, struct stat *statbuf);
	int sys_fstat(int fd, struct stat *statbuf);
	int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length);
	int sys_ftruncate(int fd, size_t size);
	int sys_fallocate(int fd, off_t offset, size_t size);
	int sys_unlink(const char *path);
	int sys_socket(int family, int type, int protocol, int *fd);
	int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length);
	int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length);
	gid_t sys_getgid();
	gid_t sys_getegid();
	uid_t sys_getuid();
	uid_t sys_geteuid();
	pid_t sys_getpid();
	pid_t sys_getppid();
	void sys_yield();
	int sys_sleep(time_t *secs, long *nanos);
	int sys_fork(pid_t *child);
	int sys_execve(const char *path, char *const argv[], char *const envp[]);
	int sys_timerfd_create(int flags, int *fd);
	int sys_timerfd_settime(int fd, int flags,
			const struct itimerspec *value);
	int sys_signalfd_create(sigset_t, int flags, int *fd);
	[[gnu::weak]] int sys_chdir(const char *path);
	int sys_chroot(const char *path);
	int sys_mkdir(const char *path);
	int sys_symlink(const char *target_path, const char *link_path);
	int sys_rename(const char *path, const char *new_path);
	int sys_fcntl(int fd, int request, va_list args, int *result);
	int sys_ttyname(int fd, char *buf, size_t size);
	[[gnu::weak]] int sys_fadvise(int fd, off_t offset, off_t length, int advice);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window);

#ifndef MLIBC_BUILDING_RTDL
	int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window);
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_vm_unmap(void *pointer, size_t size);

[[gnu::weak]] int sys_vm_readahead(void *pointer, size_t size);

#ifndef MLIBC_BUILDING_RTDL
	int sys_tcgetattr(int fd, struct termios *attr);
	int sys_tcsetattr(int, int, const struct termios *attr);
	int sys_pipe(int *fds);
	int sys_socketpair(int domain, int type_and_flags, int proto, int *fds);
	int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events);
	int sys_epoll_create(int flags, int *fd);
	int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev);
	int sys_epoll_wait(int epfd, struct epoll_event *evnts, int n, int timeout, int *raised);
	int sys_inotify_create(int flags, int *fd);
	int sys_ioctl(int fd, unsigned long request, void *arg, int *result);
	int sys_getsockopt(int fd, int layer, int number,
			void *__restrict buffer, socklen_t *__restrict size);
	int sys_setsockopt(int fd, int layer, int number,
			const void *buffer, socklen_t size);
	int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid);
	int sys_mount(const char *source, const char *target,
			const char *fstype, unsigned long flags, const void *data);
	int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve);
	int sys_sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict);
	int sys_kill(int, int);
	int sys_accept(int fd, int *newfd);
	int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
	int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
	int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
			socklen_t *actual_length);
	[[gnu::weak]] int sys_gethostname(char *buffer, size_t bufsize);
#endif // !defined(MLIBC_BUILDING_RTDL)

} //namespace mlibc

