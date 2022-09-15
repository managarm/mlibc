#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <stdlib.h>
#include <abi-bits/fcntl.h>
#include <lyre/syscall.h>
#include <frg/hash.hpp>
#include <frg/hash_map.hpp>

#define STRINGIFY_(X) #X
#define STRINGIFY(X) STRINGIFY_(X)
#define STUB_ONLY { \
	sys_libc_log("STUB_ONLY function on line " STRINGIFY(__LINE__) " was called"); \
	sys_libc_panic(); \
}

namespace mlibc {

void sys_libc_log(const char *message) {
	__syscall(SYS_debug, message);
}

void sys_libc_panic() {
	sys_libc_log("\nMLIBC PANIC\n");
	sys_exit(1);
	__builtin_unreachable();
}

void sys_exit(int status) {
	__syscall(SYS_exit, status);
	__builtin_unreachable();
}

#ifndef MLIBC_BUILDING_RTDL

int sys_kill(pid_t, int) STUB_ONLY

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
	__syscall(SYS_set_fs_base, pointer);
	return 0;
}

#ifndef MLIBC_BUILDING_RTDL

int sys_ppoll(struct pollfd *fds, int nfds, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
	__syscall_ret ret = __syscall(SYS_ppoll, fds, nfds, timeout, sigmask);
	int ret_value = (int)ret.ret;

	if (ret_value == -1)
		return ret.errno;

	*num_events = ret_value;
	return 0;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	struct timespec ts;
	ts.tv_sec = timeout / 1000;
	ts.tv_nsec = (timeout % 1000) * 1000000;
	return sys_ppoll(fds, count, &ts, NULL, num_events);
}

int sys_epoll_pwait(int, struct epoll_event *, int,
					int, const sigset_t *, int *) STUB_ONLY

int sys_epoll_create(int, int *) STUB_ONLY

int sys_epoll_ctl(int, int, int, struct epoll_event *) STUB_ONLY

int sys_pselect(int nfds, fd_set *read_set, fd_set *write_set,
		fd_set *except_set, const struct timespec *timeout,
		const sigset_t *sigmask, int *num_events) {
	struct pollfd *fds = (struct pollfd *)calloc(nfds, sizeof(struct pollfd));
	if (fds == NULL) {
		return ENOMEM;
	}

	for (int i = 0; i < nfds; i++) {
		struct pollfd *fd = &fds[i];

		if (read_set && FD_ISSET(i, read_set)) {
			fd->events |= POLLIN;
		}
		if (write_set && FD_ISSET(i, write_set)) {
			fd->events |= POLLOUT;
		}
		if (except_set && FD_ISSET(i, except_set)) {
			fd->events |= POLLPRI;
		}

		if (!fd->events) {
			fd->fd = -1;
			continue;
		}
		fd->fd = i;
	}

	int ret = sys_ppoll(fds, nfds, timeout, sigmask, num_events);
	if (ret != 0) {
		free(fds);
		return ret;
	}

	fd_set res_read_set, res_write_set, res_except_set;
	FD_ZERO(&res_read_set);
	FD_ZERO(&res_write_set);
	FD_ZERO(&res_except_set);

	for (int i = 0; i < nfds; i++) {
		struct pollfd *fd = &fds[i];

		if (read_set && FD_ISSET(i, read_set) && (fd->revents & (POLLIN | POLLERR | POLLHUP)) != 0) {
			FD_SET(i, &res_read_set);
		}
		if (write_set && FD_ISSET(i, write_set) && (fd->revents & (POLLOUT | POLLERR | POLLHUP)) != 0) {
			FD_SET(i, &res_write_set);
		}
		if (except_set && FD_ISSET(i, except_set) && (fd->revents & POLLPRI) != 0) {
			FD_SET(i, &res_except_set);
		}
	}

	free(fds);
	if (read_set) {
		*read_set = res_read_set;
	}
	if (write_set) {
		*write_set = res_write_set;
	}
	if (except_set) {
		*except_set = res_except_set;
	}

	return 0;
}

#endif

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	__syscall_ret ret = __syscall(SYS_futex_wait, pointer, expected, time);

	if ((int)ret.ret == -1)
		return ret.errno;

	return 0;
}

int sys_futex_wake(int *pointer) {
	__syscall_ret ret = __syscall(SYS_futex_wake, pointer);

	if ((int)ret.ret == -1)
		return ret.errno;

	int num_woken = ret.ret;

	__ensure(num_woken >= 0 && num_woken <= 1);
	return num_woken;
}

#ifndef MLIBC_BUILDING_RTDL

int sys_timerfd_create(int, int *) STUB_ONLY

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
	__syscall_ret ret = __syscall(SYS_ioctl, fd, request, arg);

	if ((int)ret.ret == -1)
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
	__syscall_ret ret = __syscall(SYS_getcwd, buffer, size);

	if ((int)ret.ret == -1)
		return ret.errno;

	return 0;
}

#endif

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	__syscall_ret ret = __syscall(SYS_openat, dirfd, path, flags, mode);

	if ((int)ret.ret == -1)
		return ret.errno;

	*fd = (int)ret.ret;
	return 0;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	return sys_openat(AT_FDCWD, path, flags, mode, fd);
}

#ifndef MLIBC_BUILDING_RTDL

int sys_open_dir(const char *path, int *handle) {
	return sys_openat(AT_FDCWD, path, O_DIRECTORY, 0, handle);
}

struct ReadDirState {
	size_t offset;
	size_t capacity;
	void *buffer;
};

static frg::hash_map<int, ReadDirState *, frg::hash<int>, MemoryAllocator> open_dirs{frg::hash<int>{}, getAllocator()};

static ReadDirState *get_dir_state(int fdnum) {
	ReadDirState *result;
	if (auto value = open_dirs.get(fdnum)) {
		result = *value;
	} else {
		result = (ReadDirState *)malloc(sizeof(ReadDirState));
		result->offset = 0;
		result->capacity = 1024;
		result->buffer = malloc(result->capacity);
		open_dirs.insert(fdnum, result);
	}
	return result;
}

int sys_read_entries(int fdnum, void *buffer, size_t max_size, size_t *bytes_read) {
	ReadDirState *state = get_dir_state(fdnum);

retry:
	__syscall_ret ret = __syscall(SYS_readdir, fdnum, state->buffer, &state->capacity);
	if ((int)ret.ret == -1) {
		if (ret.errno == ENOBUFS) {
			state->buffer = realloc(state->buffer, state->capacity);
			goto retry;
		} else {
			return ret.errno;
		}
	}

	size_t offset = 0;
	while (offset < max_size) {
		struct dirent *ent = (struct dirent *)((char *)state->buffer + state->offset);
		if (ent->d_reclen == 0) {
			break;
		}

		if (offset + ent->d_reclen >= max_size) {
			break;
		}

		memcpy((char *)buffer + offset, ent, ent->d_reclen);
		offset += ent->d_reclen;
		state->offset += ent->d_reclen;
	}

	*bytes_read = offset;
	return 0;
}

#endif

int sys_close(int fd) {
	__syscall_ret ret = __syscall(SYS_close, fd);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
#ifndef MLIBC_BUILDING_RTDL
	open_dirs.remove(fd);
#endif
	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	__syscall_ret ret = __syscall(SYS_seek, fd, offset, whence);
	off_t ret_value = (off_t)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*new_offset = ret_value;
	return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	__syscall_ret ret = __syscall(SYS_read, fd, buf, count);
	ssize_t ret_value = (ssize_t)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*bytes_read = ret_value;
	return 0;
}

#ifndef MLIBC_BUILDING_RTDL

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	__syscall_ret ret = __syscall(SYS_write, fd, buf, count);
	ssize_t ret_value = (ssize_t)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*bytes_written = ret_value;
	return 0;
}

int sys_readlink(const char *path, void *data, size_t max_size, ssize_t *length) {
	__syscall_ret ret = __syscall(SYS_readlinkat, AT_FDCWD, path, data, max_size);
	ssize_t ret_value = (ssize_t)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*length = ret_value;
	return 0;
}

int sys_link(const char *old_path, const char *new_path) {
	return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	__syscall_ret ret = __syscall(SYS_linkat, olddirfd, old_path, newdirfd, new_path, flags);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_unlinkat(int fd, const char *path, int flags) {
	__syscall_ret ret = __syscall(SYS_unlinkat, fd, path, flags);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
	__syscall_ret ret = __syscall(SYS_fchmodat, fd, pathname, mode, flags);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_fchmod(int fd, mode_t mode) {
	return sys_fchmodat(fd, "", mode, AT_EMPTY_PATH);
}

int sys_chmod(const char *pathname, mode_t mode) {
	return sys_fchmodat(AT_FDCWD, pathname, mode, 0);
}

int sys_rmdir(const char *) STUB_ONLY

#endif

int sys_vm_map(void *hint, size_t size, int prot, int flags,
			   int fd, off_t offset, void **window) {
	__syscall_ret ret = __syscall(SYS_mmap, hint, size, (uint64_t)prot << 32 | flags, fd, offset);
	void *ret_value = (void *)ret.ret;
	if (ret_value == MAP_FAILED) {
		return ret.errno;
	}
	*window = ret_value;
	return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
	__syscall_ret ret = __syscall(SYS_unmmap, pointer, size);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

#ifndef MLIBC_BUILDING_RTDL

int sys_vm_protect(void *, size_t, int) STUB_ONLY

#endif

int sys_anon_allocate(size_t size, void **pointer) {
	return sys_vm_map(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0, pointer);
}

int sys_anon_free(void *pointer, size_t size) {
	return sys_vm_unmap(pointer, size);
}

#ifndef MLIBC_BUILDING_RTDL

pid_t sys_getpid() {
	__syscall_ret ret = __syscall(SYS_getpid);
	return (pid_t)ret.ret;
}

pid_t sys_getppid() {
	return 0;
}

uid_t sys_getuid() {
	return 0;
}

uid_t sys_geteuid() {
	return 0;
}

gid_t sys_getgid() {
	return 0;
}

int sys_setgid(gid_t) {
	return 0;
}

pid_t sys_getpgid(pid_t, pid_t *) {
	return 0;
}

gid_t sys_getegid() {
	return 0;
}

int sys_setpgid(pid_t, pid_t) {
	return 0;
}

int sys_ttyname(int, char *, size_t) {
	return ENOSYS;
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	struct timespec buf;
	__syscall_ret ret = __syscall(SYS_getclock, clock, &buf);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	*secs = buf.tv_sec;
	*nanos = buf.tv_nsec;
	return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	__syscall_ret ret;
	switch (fsfdt) {
		case fsfd_target::fd:
			ret = __syscall(SYS_stat, fd, "", flags | AT_EMPTY_PATH, statbuf);
			break;
		case fsfd_target::path:
			ret = __syscall(SYS_stat, AT_FDCWD, path, flags, statbuf);
			break;
		case fsfd_target::fd_path:
			ret = __syscall(SYS_stat, fd, path, flags, statbuf);
			break;
		default:
			__ensure(!"sys_stat: Invalid fsfdt");
			__builtin_unreachable();
	}
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
	(void)flags;
	struct stat buf;
	if (int r = sys_stat(fsfd_target::fd_path, dirfd, pathname, mode & AT_SYMLINK_FOLLOW, &buf)) {
		return r;
	}
	return 0;
}

int sys_access(const char *path, int mode) {
	return sys_faccessat(AT_FDCWD, path, mode, 0);
}

int sys_pipe(int *fds, int flags) {
	__syscall_ret ret = __syscall(SYS_pipe, fds, flags);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_chdir(const char *path) {
	__syscall_ret ret = __syscall(SYS_chdir, path);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_mkdir(const char *path, mode_t mode) {
	return sys_mkdirat(AT_FDCWD, path, mode);
}

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	__syscall_ret ret = __syscall(SYS_mkdirat, dirfd, path, mode);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_socket(int domain, int type_and_flags, int proto, int *fd) {
	__syscall_ret ret = __syscall(SYS_socket, domain, type_and_flags, proto);
	int ret_value = (int)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*fd = ret_value;
	return 0;
}

int sys_socketpair(int, int, int, int *) STUB_ONLY

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	__syscall_ret ret = __syscall(SYS_bind, fd, addr_ptr, addr_length);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	__syscall_ret ret = __syscall(SYS_connect, fd, addr_ptr, addr_length);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length) {
	__syscall_ret ret = __syscall(SYS_accept, fd, addr_ptr, addr_length);
	int ret_value = (int)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*newfd = ret_value;
	return 0;
}

int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
	(void)fd;
	(void)size;
	if (layer == SOL_SOCKET && number == SO_PEERCRED) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_PEERCRED is unimplemented" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_SNDBUF is unimplemented" << frg::endlog;
		*(int *)buffer = 4096;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_TYPE) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_TYPE is unimplemented, hardcoding SOCK_STREAM" << frg::endlog;
		*(int *)buffer = SOCK_STREAM;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_ERROR) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_ERROR is unimplemented, hardcoding 0" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_KEEPALIVE is unimplemented, hardcoding 0" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else {
		mlibc::panicLogger() << "mlibc: Unexpected getsockopt() call, layer: " << layer << " number: " << number << frg::endlog;
	}

	return 0;
}

int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
	(void)fd;
	(void)buffer;
	(void)size;
	if (layer == SOL_SOCKET && number == SO_PASSCRED) {
		mlibc::infoLogger() << "mlibc: setsockopt(SO_PASSCRED) is not implemented correctly" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_ATTACH_FILTER) {
		mlibc::infoLogger() << "mlibc: setsockopt(SO_ATTACH_FILTER) is not implemented correctly" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_RCVBUFFORCE) {
		mlibc::infoLogger() << "mlibc: setsockopt(SO_RCVBUFFORCE) is not implemented correctly" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
		mlibc::infoLogger() << "mlibc: setsockopt() call with SOL_SOCKET and SO_SNDBUF is unimplemented" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "mlibc: setsockopt() call with SOL_SOCKET and SO_KEEPALIVE is unimplemented" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_REUSEADDR) {
		mlibc::infoLogger() << "mlibc: setsockopt() call with SOL_SOCKET and SO_REUSEADDR is unimplemented" << frg::endlog;
		return 0;
	} else if (layer == AF_NETLINK && number == SO_ACCEPTCONN) {
		mlibc::infoLogger() << "mlibc: setsockopt() call with AF_NETLINK and SO_ACCEPTCONN is unimplemented" << frg::endlog;
		return 0;
	} else {
		mlibc::panicLogger() << "mlibc: Unexpected setsockopt() call, layer: " << layer << " number: " << number << frg::endlog;
		sys_libc_panic();
	}
}

int sys_msg_recv(int sockfd, struct msghdr *hdr, int flags, ssize_t *length) {
	__syscall_ret ret = __syscall(SYS_recvmsg, sockfd, hdr, flags);
	ssize_t ret_value = (ssize_t)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*length = ret_value;
	return 0;
}

int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
	__syscall_ret ret = __syscall(SYS_getpeername, fd, addr_ptr, &max_addr_length);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	*actual_length = max_addr_length;
	return 0;
}

int sys_listen(int fd, int backlog) {
	__syscall_ret ret = __syscall(SYS_listen, fd, backlog);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_inotify_create(int, int *) STUB_ONLY

int sys_fork(pid_t *child) {
	__syscall_ret ret = __syscall(SYS_fork);
	pid_t ret_value = (pid_t)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*child = ret_value;
	return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	__syscall_ret ret = __syscall(SYS_exec, path, argv, envp);
	return ret.errno;
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
	__syscall_ret ret = __syscall(SYS_fcntl, fd, request, va_arg(args, uint64_t));
	int ret_value = (int)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*result = ret_value;
	return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
	(void)flags;
	__syscall_ret ret = __syscall(SYS_fcntl, fd, F_DUPFD, 0);
	int ret_value = (int)ret.ret;
	if (ret_value == -1) {
		return ret.errno;
	}
	*newfd = ret_value;
	return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
	__syscall_ret ret = __syscall(SYS_dup3, fd, newfd, flags);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_sigprocmask(int, const sigset_t *__restrict, sigset_t *__restrict) {
	mlibc::infoLogger() << "mlibc: sys_sigprocmask() is a stub" << frg::endlog;
	return 0;
}

int sys_sigaction(int, const struct sigaction *, struct sigaction *) {
	mlibc::infoLogger() << "mlibc: sys_sigaction() is a stub" << frg::endlog;
	return 0;
}

int sys_signalfd_create(sigset_t, int, int *) STUB_ONLY

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	if (ru != NULL) {
		mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported" << frg::endlog;
		return ENOSYS;
	}
again:
	__syscall_ret ret = __syscall(SYS_waitpid, pid, status, flags);
	pid_t ret_value = (pid_t)ret.ret;
	if (ret_value == -1) {
		if (ret.errno == EINTR) {
			goto again;
		}
		return ret.errno;
	}
	*ret_pid = ret_value;
	return 0;
}

int sys_getgroups(size_t, const gid_t *, int *) STUB_ONLY

int sys_mount(const char *, const char *, const char *, unsigned long, const void *) STUB_ONLY

int sys_umount2(const char *, int) STUB_ONLY

int sys_gethostname(char *buffer, size_t bufsize) {
	struct utsname utsname;
	if (int err = sys_uname(&utsname)) {
		return err;
	}
	if (strlen(utsname.nodename) >= bufsize) {
		return ENAMETOOLONG;
	}
	strncpy(buffer, utsname.nodename, bufsize);
	return 0;
}

int sys_sethostname(const char *, size_t) STUB_ONLY

int sys_sleep(time_t *secs, long *nanos) {
	struct timespec time = {.tv_sec = *secs, .tv_nsec = *nanos};
	struct timespec rem = {.tv_sec = 0, .tv_nsec = 0};
	__syscall_ret ret = __syscall(SYS_sleep, &time, &rem);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
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

int sys_umask(mode_t mode, mode_t *old) {
	__syscall_ret ret = __syscall(SYS_umask, mode);
	*old = (mode_t)ret.ret;
	return 0;
}

int sys_uname(struct utsname *buf) {
	__syscall_ret ret = __syscall(SYS_uname, buf);
	if ((int)ret.ret == -1) {
		return ret.errno;
	}
	return 0;
}

int sys_fsync(int) {
	mlibc::infoLogger() << "sys_fsync is a stub" << frg::endlog;
	return 0;
}

#endif

} // namespace mlibc
