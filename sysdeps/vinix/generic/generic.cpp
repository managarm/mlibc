#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <stdlib.h>

#include "syscall.h"

#define STUB_ONLY {                             \
	__ensure(!"STUB_ONLY function was called"); \
	__builtin_unreachable();                    \
}

#ifndef MLIBC_BUILDING_RTLD

namespace {

int fcntl_helper(int fd, int request, int *result, ...) {
	va_list args;
	va_start(args, result);
	if(!mlibc::sys_fcntl) {
		return ENOSYS;
	}
	int ret = mlibc::sys_fcntl(fd, request, args, result);
	va_end(args);
	return ret;
}

}

#endif

namespace mlibc {

void sys_libc_log(const char *message) {
	__syscall(0, message);
}

void sys_libc_panic() {
	sys_libc_log("\nMLIBC PANIC\n");
	sys_exit(1);
	__builtin_unreachable();
}

void sys_exit(int status) {
	__syscall(15, status);
	__builtin_unreachable();
}

#ifndef MLIBC_BUILDING_RTLD

[[noreturn]] void sys_thread_exit() {
	for (;;);
	__builtin_unreachable();
}

extern "C" void __mlibc_thread_entry();

int sys_clone(void *tcb, pid_t *pid_out, void *stack) {
	(void)tcb;

	__syscall_ret ret = __syscall(65, (uintptr_t)__mlibc_thread_entry, (uintptr_t)stack);
	int ret_value = (int)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}

	*pid_out = ret_value;
	return 0;
}

int sys_kill(pid_t pid, int signal) {
	__syscall_ret ret = __syscall(26, pid, signal);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_tcgetattr(int fd, struct termios *attr) {
	int ret;

	if (int r = sys_ioctl(fd, TCGETS, attr, &ret) != 0) {
		return r;
	}

	return 0;
}

int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
	int ret;

	switch (optional_action) {
		case TCSANOW:
			optional_action = TCSETS; break;
		case TCSADRAIN:
			optional_action = TCSETSW; break;
		case TCSAFLUSH:
			optional_action = TCSETSF; break;
		default:
			__ensure(!"Unsupported tcsetattr");
	}

	if (int r = sys_ioctl(fd, optional_action, (void *)attr, &ret) != 0) {
		return r;
	}

	return 0;
}

#endif

int sys_tcb_set(void *pointer) {
	__syscall(7, pointer);
	return 0;
}

#ifndef MLIBC_BUILDING_RTLD

int sys_ppoll(struct pollfd *fds, int nfds, const struct timespec *timeout,
		const sigset_t *sigmask, int *num_events) {
	__syscall_ret ret = __syscall(36, fds, nfds, timeout, sigmask);

	if (ret.errno != 0)
		return ret.errno;

	*num_events = (int)ret.ret;
	return 0;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	struct timespec ts;
	ts.tv_sec = timeout / 1000;
	ts.tv_nsec = (timeout % 1000) * 1000000;
	return sys_ppoll(fds, count, timeout < 0 ? NULL : &ts, NULL, num_events);
}

int sys_pselect(int nfds, fd_set *read_set, fd_set *write_set,
		fd_set *except_set, const struct timespec *timeout,
		const sigset_t *sigmask, int *num_events) {
	struct pollfd *fds = (struct pollfd *)calloc(nfds, sizeof(struct pollfd));
	if (fds == NULL) {
		return ENOMEM;
	}

	for (int i = 0; i < nfds; i++) {
		struct pollfd *fd = &fds[i];

		if (read_set && FD_ISSET(i, read_set))
			fd->events |= POLLIN; // TODO: Additional events.
		if (write_set && FD_ISSET(i, write_set))
			fd->events |= POLLOUT; // TODO: Additional events.
		if (except_set && FD_ISSET(i, except_set))
			fd->events |= POLLPRI;

		if (!fd->events) {
			fd->fd = -1;
			continue;
		}

		fd->fd = i;
	}

	int e = sys_ppoll(fds, nfds, timeout, sigmask, num_events);

	if (e != 0) {
		free(fds);
		return e;
	}

	fd_set res_read_set;
	fd_set res_write_set;
	fd_set res_except_set;
	FD_ZERO(&res_read_set);
	FD_ZERO(&res_write_set);
	FD_ZERO(&res_except_set);

	for (int i = 0; i < nfds; i++) {
		struct pollfd *fd = &fds[i];

		if (read_set && FD_ISSET(i, read_set)
				&& fd->revents & (POLLIN | POLLERR | POLLHUP)) {
			FD_SET(i, &res_read_set);
		}

		if (write_set && FD_ISSET(i, write_set)
				&& fd->revents & (POLLOUT | POLLERR | POLLHUP)) {
			FD_SET(i, &res_write_set);
		}

		if (except_set && FD_ISSET(i, except_set)
				&& fd->revents & POLLPRI) {
			FD_SET(i, &res_except_set);
		}
	}

	free(fds);

	if (read_set)
		*read_set = res_read_set;
	if (write_set)
		*write_set = res_write_set;
	if (except_set)
		*except_set = res_except_set;

	return 0;
}

#endif

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	__syscall_ret ret = __syscall(23, pointer, expected, time);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_futex_wake(int *pointer) {
	__syscall_ret ret = __syscall(24, pointer);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

#ifndef MLIBC_BUILDING_RTLD

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
	__syscall_ret ret = __syscall(9, fd, request, arg);

	if (ret.errno != 0)
		return ret.errno;

	*result = (int)ret.ret;
	return 0;
}

int sys_isatty(int fd) {
	struct winsize ws;
	int ret;

	if (!sys_ioctl(fd, TIOCGWINSZ, &ws, &ret))
		return 0;

	return ENOTTY;
}

int sys_getcwd(char *buffer, size_t size) {
	__syscall_ret ret = __syscall(25, buffer, size);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

#endif

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	__syscall_ret ret = __syscall(2, dirfd, path, flags, mode);

	if (ret.errno != 0)
		return ret.errno;

	*fd = (int)ret.ret;
	return 0;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	return sys_openat(AT_FDCWD, path, flags, mode, fd);
}

#ifndef MLIBC_BUILDING_RTLD

int sys_open_dir(const char *path, int *handle) {
	return sys_open(path, O_DIRECTORY, 0, handle);
}

int sys_read_entries(int fd, void *buffer, size_t max_size, size_t *bytes_read) {
	(void)max_size;
	__syscall_ret ret = __syscall(19, fd, buffer);

	if (ret.ret == (uint64_t)-1 && ret.errno == 0) {
		// End of directory.
		*bytes_read = 0;
		return 0;
	} else if (ret.errno != 0) {
		return ret.errno;
	}

	*bytes_read = sizeof(struct dirent);
	return 0;
}

#endif

int sys_close(int fd) {
	__syscall_ret ret = __syscall(6, fd);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	__syscall_ret ret = __syscall(5, fd, offset, whence);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*new_offset = (off_t)ret.ret;
	return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	__syscall_ret ret = __syscall(3, fd, buf, count);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*bytes_read = (ssize_t)ret.ret;
	return 0;
}

#ifndef MLIBC_BUILDING_RTLD

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	__syscall_ret ret = __syscall(4, fd, buf, count);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*bytes_written = (ssize_t)ret.ret;
	return 0;
}

int sys_readlink(const char *path, void *data, size_t max_size, ssize_t *length) {
	__syscall_ret ret = __syscall(33, AT_FDCWD, path, data, max_size);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*length = (ssize_t)ret.ret;
	return 0;
}

int sys_link(const char *old_path, const char *new_path) {
	return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	__syscall_ret ret = __syscall(58, olddirfd, old_path, newdirfd, new_path, flags);

	if (ret.errno != 0) {
		return ret.errno;
	}

	return 0;
}

int sys_unlinkat(int fd, const char *path, int flags) {
	__syscall_ret ret = __syscall(35, fd, path, flags);

	if (ret.errno != 0) {
		return ret.errno;
	}

	return 0;
}

int sys_fchmod(int fd, mode_t mode) {
	__syscall_ret ret = __syscall(57, fd, mode);

	if (ret.errno != 0) {
		return ret.errno;
	}

	return 0;
}

int sys_rmdir(const char *path) {
	__syscall_ret ret = __syscall(37, AT_FDCWD, path);

	if (ret.errno != 0) {
		return ret.errno;
	}

	return 0;
}

#endif

int sys_vm_map(void *hint, size_t size, int prot, int flags,
			   int fd, off_t offset, void **window) {
	__syscall_ret ret = __syscall(1, hint, size,
								  (uint64_t)prot << 32 | (uint64_t)flags, fd, offset);
	if (ret.errno != 0)
		return ret.errno;

	*window = (void *)ret.ret;
	return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
	__syscall_ret ret = __syscall(34, pointer, size);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
	__syscall_ret ret = __syscall(48, pointer, size, prot);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_anon_allocate(size_t size, void **pointer) {
	return sys_vm_map(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE,
					  MAP_ANONYMOUS, -1, 0, pointer);
}

int sys_anon_free(void *pointer, size_t size) {
	return sys_vm_unmap(pointer, size);
}

#ifndef MLIBC_BUILDING_RTLD

pid_t sys_getpid() {
	__syscall_ret ret = __syscall(31);

	return ret.ret;
}

pid_t sys_getppid() {
	__syscall_ret ret = __syscall(32);

	return ret.ret;
}

uid_t sys_getuid() {
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return 0;
}

uid_t sys_geteuid() {
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return 0;
}

gid_t sys_getgid() {
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return 0;
}

int sys_setgid(gid_t gid) {
	(void)gid;
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return 0;
}

pid_t sys_getpgid(pid_t pid, pid_t *pgid) {
	(void)pid;
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	*pgid = 0;
	return 0;
}

gid_t sys_getegid() {
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return 0;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	(void)pid; (void)pgid;
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return 0;
}

int sys_ttyname(int fd, char *buf, size_t size) {
	(void)fd; (void)buf; (void)size;
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return ENOSYS;
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	struct timespec ts;
	__syscall_ret ret = __syscall(50, clock, &ts);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*secs = ts.tv_sec;
	*nanos = ts.tv_nsec;

	return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	__syscall_ret ret;
	switch (fsfdt) {
		case fsfd_target::fd: {
			ret = __syscall(10, fd, statbuf);
			break;
		}
		case fsfd_target::path: {
			ret = __syscall(11, AT_FDCWD, path, statbuf, flags);
			break;
		}
		case fsfd_target::fd_path: {
			ret = __syscall(11, fd, path, statbuf, flags);
			break;
		}
		default: {
			__ensure(!"stat: Invalid fsfdt");
			__builtin_unreachable();
		}
	}
	if (ret.errno != 0)
		return ret.errno;
	return ret.ret;
}

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
	__syscall_ret ret = __syscall(20, dirfd, pathname, mode, flags);

	if (ret.errno != 0) {
		return ret.errno;
	}

	return 0;
}

int sys_access(const char *path, int mode) {
	return sys_faccessat(AT_FDCWD, path, mode, 0);
}

int sys_pipe(int *fds, int flags) {
	__syscall_ret ret = __syscall(21, fds, flags);

	if (ret.errno != 0) {
		return ret.errno;
	}

	return 0;
}

int sys_chdir(const char *path) {
	__syscall_ret ret = __syscall(18, path);

	if (ret.errno != 0) {
		return ret.errno;
	}

	return 0;
}

int sys_mkdir(const char *path, mode_t mode) {
	return sys_mkdirat(AT_FDCWD, path, mode);
}

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	__syscall_ret ret = __syscall(22, dirfd, path, mode);

	return ret.errno;
}

int sys_socket(int domain, int type_and_flags, int proto, int *fd) {
	__syscall_ret ret = __syscall(39, domain, type_and_flags, proto);

	if (ret.errno != 0)
		return ret.errno;

	*fd = (int)ret.ret;
	return 0;
}

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
	__syscall_ret ret = __syscall(46, domain, type_and_flags, proto, fds);

	return ret.errno;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	__syscall_ret ret = __syscall(40, fd, addr_ptr, addr_length);

	return ret.errno;
}

int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	__syscall_ret ret = __syscall(59, fd, addr_ptr, addr_length);

	return ret.errno;
}

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	__syscall_ret ret = __syscall(61, fd, addr_ptr, addr_length);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*newfd = ret.ret;

	if(flags & SOCK_NONBLOCK) {
		int fcntl_ret = 0;
		fcntl_helper(*newfd, F_GETFL, &fcntl_ret);
		fcntl_helper(*newfd, F_SETFL, &fcntl_ret, fcntl_ret | O_NONBLOCK);
	}

	if(flags & SOCK_CLOEXEC) {
		int fcntl_ret = 0;
		fcntl_helper(*newfd, F_GETFD, &fcntl_ret);
		fcntl_helper(*newfd, F_SETFD, &fcntl_ret, fcntl_ret | FD_CLOEXEC);
	}

	return 0;
}

int sys_getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) {
	(void)fd; (void)size;
	if(layer == SOL_SOCKET && number == SO_PEERCRED) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_PEERCRED is unimplemented\e[39m" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_SNDBUF) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_SNDBUF is unimplemented\e[39m" << frg::endlog;
		*(int *)buffer = 4096;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_TYPE) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_TYPE is unimplemented, hardcoding SOCK_STREAM\e[39m" << frg::endlog;
		*(int *)buffer = SOCK_STREAM;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_ERROR) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_ERROR is unimplemented, hardcoding 0\e[39m" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "\e[31mmlibc: getsockopt() call with SOL_SOCKET and SO_KEEPALIVE is unimplemented, hardcoding 0\e[39m" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	}else{
		mlibc::panicLogger() << "\e[31mmlibc: Unexpected getsockopt() call, layer: " << layer << " number: " << number << "\e[39m" << frg::endlog;
		__builtin_unreachable();
	}

	return 0;
}


int sys_setsockopt(int fd, int layer, int number,
		const void *buffer, socklen_t size) {
	(void)fd; (void)buffer; (void)size;
	if(layer == SOL_SOCKET && number == SO_PASSCRED) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt(SO_PASSCRED) is not implemented"
				" correctly\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_ATTACH_FILTER) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt(SO_ATTACH_FILTER) is not implemented"
				" correctly\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_RCVBUFFORCE) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt(SO_RCVBUFFORCE) is not implemented"
				" correctly\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_SNDBUF) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_SNDBUF is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_KEEPALIVE is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == SOL_SOCKET && number == SO_REUSEADDR) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with SOL_SOCKET and SO_REUSEADDR is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else if(layer == AF_NETLINK && number == SO_ACCEPTCONN) {
		mlibc::infoLogger() << "\e[31mmlibc: setsockopt() call with AF_NETLINK and SO_ACCEPTCONN is unimplemented\e[39m" << frg::endlog;
		return 0;
	}else{
		mlibc::panicLogger() << "\e[31mmlibc: Unexpected setsockopt() call, layer: " << layer << " number: " << number << "\e[39m" << frg::endlog;
		__builtin_unreachable();
	}
}

int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length) {
	__syscall_ret ret = __syscall(62, sockfd, hdr, flags);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*length = (ssize_t)ret.ret;
	return 0;
}

int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
	__syscall_ret ret = __syscall(60, fd, addr_ptr, &max_addr_length);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*actual_length = max_addr_length;
	return 0;
}

int sys_listen(int fd, int backlog) {
	__syscall_ret ret = __syscall(41, fd, backlog);

	return ret.errno;
}

int sys_inotify_create(int flags, int *fd) {
	__syscall_ret ret = __syscall(42, flags);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*fd = (int)ret.ret;
	return 0;
}

int sys_fork(pid_t *child) {
	__syscall_ret ret = __syscall(14);

	if (ret.errno != 0) {
		return ret.errno;
	}

	*child = (pid_t)ret.ret;
	return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	__syscall_ret ret = __syscall(17, path, argv, envp);

	return ret.errno;
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
	__syscall_ret ret = __syscall(12, fd, request, va_arg(args, uint64_t));

	if (ret.errno != 0)
		return ret.errno;

	*result = (ssize_t)ret.ret;
	return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
	(void)flags;
	__syscall_ret ret = __syscall(12, fd, F_DUPFD, 0);

	if (ret.errno != 0)
		return ret.errno;

	*newfd = (ssize_t)ret.ret;
	return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
	__syscall_ret ret = __syscall(13, fd, newfd, flags);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	__syscall_ret ret = __syscall(28, how, set, retrieve);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	__syscall_ret ret = __syscall(29, signum, act, oldact);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_signalfd_create(sigset_t mask, int flags, int *fd) {
	__syscall_ret ret = __syscall(45, *fd, mask, flags);

	if (ret.errno != 0)
		return ret.errno;

	*fd = (int)ret.ret;
	return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	if(ru) {
		mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported" << frg::endlog;
		return ENOSYS;
	}

again:
	__syscall_ret ret = __syscall(16, pid, status, flags);

	if (ret.errno != 0) {
		if (ret.errno == EINTR) {
			goto again;
		}

		return ret.errno;
	}

	*ret_pid = (pid_t)ret.ret;
	return 0;
}

int sys_getgroups(size_t size, gid_t *list, int *_ret) {
	__syscall_ret ret = __syscall(38, size, list);

	if (ret.errno != 0)
		return ret.errno;

	*_ret = (int)ret.ret;
	return 0;
}

int sys_mount(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data) {
	__syscall_ret ret = __syscall(43, source, target, fstype, flags, data);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_umount2(const char *target, int flags) {
	__syscall_ret ret = __syscall(44, target, flags);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_gethostname(char *buffer, size_t bufsize) {
	__syscall_ret ret = __syscall(51, buffer, bufsize);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_sethostname(const char *buffer, size_t bufsize) {
	__syscall_ret ret = __syscall(52, buffer, bufsize);

	if (ret.errno != 0)
		return ret.errno;

	return 0;
}

int sys_sleep(time_t *secs, long *nanos) {
	struct timespec req = {
		.tv_sec = *secs,
		.tv_nsec = *nanos
	};
	struct timespec rem = {0, 0};

	__syscall_ret ret = __syscall(53, &req, &rem);

	if (ret.errno != 0)
		return ret.errno;

	*secs = rem.tv_sec;
	*nanos = rem.tv_nsec;
	return 0;
}

int sys_getitimer(int, struct itimerval *) {
	mlibc::infoLogger() << "mlibc: sys_getitimer() is unimplemented" << frg::endlog;
	return ENOSYS;
}

int sys_setitimer(int, const struct itimerval *, struct itimerval *) {
	mlibc::infoLogger() << "mlibc: sys_setitimer() is unimplemented" << frg::endlog;
	return ENOSYS;
}

#endif

} // namespace mlibc
