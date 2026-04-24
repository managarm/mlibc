#include <abi-bits/limits.h>
#include <abi-bits/pid_t.h>
#include <errno.h>
#include <frg/logging.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/tcb.hpp>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <zinnia/syscall.hpp>

namespace mlibc {

[[noreturn]] void Sysdeps<Exit>::operator()(int status) {
	zinnia_syscall(SYSCALL_EXIT, status);
	__builtin_unreachable();
}

[[noreturn]] void Sysdeps<ThreadExit>::operator()() {
	zinnia_syscall(SYSCALL_THREAD_EXIT, 0);
	__builtin_unreachable();
}

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
	struct timespec ts;
	auto r = zinnia_syscall(SYSCALL_CLOCK_GET, clock, (size_t)&ts);
	if (r.error)
		return r.error;
	*secs = ts.tv_sec;
	*nanos = ts.tv_nsec;
	return 0;
}

int Sysdeps<ClockGetres>::operator()(int clock, time_t *secs, long *nanos) {
	struct timespec ts;
	auto r = zinnia_syscall(SYSCALL_CLOCK_GETRES, clock, (size_t)&ts);
	if (r.error)
		return r.error;
	*secs = ts.tv_sec;
	*nanos = ts.tv_nsec;
	return 0;
}

int Sysdeps<Flock>::operator()(int fd, int options) {
	auto r = zinnia_syscall(SYSCALL_FLOCK, fd, options);
	return r.error;
}

int Sysdeps<OpenDir>::operator()(const char *path, int *handle) {
	return sysdep<Open>(path, O_RDONLY | O_DIRECTORY, 0600, handle);
}

int
Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	auto r = zinnia_syscall(SYSCALL_GETDENTS, handle, (size_t)buffer, (size_t)max_size);
	if (r.error) {
		return r.error;
	}
	*bytes_read = r.value;
	return 0;
}

int
Sysdeps<Writev>::operator()(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_written) {
	if (iovc < 0)
		return EINVAL;
	if (iovc == 0) {
		*bytes_written = 0;
		return 0;
	}

	auto r = zinnia_syscall(SYSCALL_WRITEV, fd, (uint64_t)iovs, (size_t)iovc);
	if (r.error)
		return r.error;
	*bytes_written = r.value;
	return 0;
}

int Sysdeps<Write>::operator()(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	const struct iovec iov = {const_cast<void *>(buf), count};
	return sysdep<Writev>(fd, &iov, 1, bytes_written);
}

int Sysdeps<Pread>::operator()(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	auto r = zinnia_syscall(SYSCALL_PREAD, fd, (size_t)buf, n, off);
	if (r.error)
		return r.error;
	*bytes_read = r.value;
	return 0;
}

int Sysdeps<Pwrite>::operator()(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	auto r = zinnia_syscall(SYSCALL_PWRITE, fd, (size_t)buf, n, off);
	if (r.error)
		return r.error;
	*bytes_read = r.value;
	return 0;
}

int Sysdeps<Access>::operator()(const char *path, int mode) {
	return sysdep<Faccessat>(AT_FDCWD, path, mode, 0);
}

int Sysdeps<Faccessat>::operator()(int dirfd, const char *pathname, int mode, int flags) {
	return zinnia_syscall(SYSCALL_FACCESSAT, dirfd, (size_t)pathname, mode, flags).error;
}

int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
	auto r = zinnia_syscall(SYSCALL_DUP, fd, flags);
	if (r.error)
		return r.error;
	*newfd = r.value;
	return 0;
}

int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
	// dup2 is roughly equal to dup3.
	return zinnia_syscall(SYSCALL_DUP3, fd, newfd, flags).error;
}

int Sysdeps<Isatty>::operator()(int fd) {
	struct winsize ws;
	// If we're a TTY then this call will succeed.
	if (!sysdep<Ioctl>(fd, TIOCGWINSZ, &ws, nullptr))
		return 0;

	return ENOTTY;
}

int Sysdeps<Statvfs>::operator()(const char *path, struct statvfs *out) {
	return zinnia_syscall(SYSCALL_STATVFS, (size_t)path, (size_t)out).error;
}

int Sysdeps<Fstatvfs>::operator()(int fd, struct statvfs *out) {
	return zinnia_syscall(SYSCALL_FSTATVFS, fd, (size_t)out).error;
}

int
Sysdeps<Readlink>::operator()(const char *path, void *buffer, size_t max_size, ssize_t *length) {
	return sysdep<Readlinkat>(AT_FDCWD, path, buffer, max_size, length);
}

int Sysdeps<Readlinkat>::operator()(
    int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length
) {
	auto r =
	    zinnia_syscall(SYSCALL_READLINKAT, dirfd, (size_t)path, (size_t)buffer, (size_t)max_size);
	if (r.error)
		return r.error;
	*length = r.value;
	return 0;
}

int Sysdeps<Rmdir>::operator()(const char *path) {
	return zinnia_syscall(SYSCALL_RMDIRAT, AT_FDCWD, (size_t)path).error;
}

int Sysdeps<Ftruncate>::operator()(int fd, size_t size) {
	return zinnia_syscall(SYSCALL_FTRUNCATE, fd, size).error;
}

int Sysdeps<Fallocate>::operator()(int fd, off_t offset, size_t size) {
	return zinnia_syscall(SYSCALL_FALLOCATE, fd, offset, size).error;
}

int Sysdeps<Unlinkat>::operator()(int fd, const char *path, int flags) {
	return zinnia_syscall(SYSCALL_UNLINKAT, fd, (size_t)path, flags).error;
}

int Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd) {
	auto r = zinnia_syscall(SYSCALL_SOCKET, family, type, protocol);
	if (r.error)
		return r.error;
	*fd = (int)r.value;
	return 0;
}

int Sysdeps<Socketpair>::operator()(int domain, int type_and_flags, int proto, int *fds) {
	auto r = zinnia_syscall(SYSCALL_SOCKETPAIR, domain, type_and_flags, proto);
	if (r.error)
		return r.error;
	fds[0] = static_cast<int>(r.value & 0xFFFFFFFF);
	fds[1] = static_cast<int>((r.value >> 32) & 0xFFFFFFFF);
	return 0;
}

int Sysdeps<MsgSend>::operator()(int fd, const struct msghdr *hdr, int flags, ssize_t *length) {
	auto r = zinnia_syscall(SYSCALL_SENDMSG, fd, (size_t)hdr, flags);
	if (r.error)
		return r.error;
	*length = (ssize_t)r.value;
	return 0;
}

int Sysdeps<MsgRecv>::operator()(int fd, struct msghdr *hdr, int flags, ssize_t *length) {
	auto r = zinnia_syscall(SYSCALL_RECVMSG, fd, (size_t)hdr, flags);
	if (r.error)
		return r.error;
	*length = (ssize_t)r.value;
	return 0;
}

int Sysdeps<Listen>::operator()(int fd, int backlog) {
	return zinnia_syscall(SYSCALL_LISTEN, fd, backlog).error;
}

int Sysdeps<GetSockopt>::operator()(
    int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size
) {
	return zinnia_syscall(SYSCALL_GETSOCKOPT, fd, layer, number, (size_t)buffer, (size_t)size)
	    .error;
}

int
Sysdeps<SetSockopt>::operator()(int fd, int layer, int number, const void *buffer, socklen_t size) {
	return zinnia_syscall(SYSCALL_SETSOCKOPT, fd, layer, number, (size_t)buffer, size).error;
}

int Sysdeps<Shutdown>::operator()(int sockfd, int how) {
	return zinnia_syscall(SYSCALL_SHUTDOWN, sockfd, how).error;
}

int Sysdeps<Accept>::operator()(
    int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags
) {
	return zinnia_syscall(
	           SYSCALL_ACCEPT, fd, (size_t)newfd, (size_t)addr_ptr, (size_t)addr_length, flags
	)
	    .error;
}

int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	return zinnia_syscall(SYSCALL_BIND, fd, (size_t)addr_ptr, addr_length).error;
}

int Sysdeps<Connect>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	return zinnia_syscall(SYSCALL_CONNECT, fd, (size_t)addr_ptr, addr_length).error;
}

int Sysdeps<Sockname>::operator()(
    int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length
) {
	auto r = zinnia_syscall(SYSCALL_GETSOCKNAME, fd, (size_t)addr_ptr, (size_t)&max_addr_length);
	if (r.error)
		return r.error;
	*actual_length = max_addr_length;
	return 0;
}

int Sysdeps<Peername>::operator()(
    int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length
) {
	auto r = zinnia_syscall(SYSCALL_GETPEERNAME, fd, (size_t)addr_ptr, (size_t)&max_addr_length);
	if (r.error)
		return r.error;
	*actual_length = (socklen_t)max_addr_length;
	return 0;
}

gid_t Sysdeps<GetGid>::operator()() { return zinnia_syscall(SYSCALL_GETGID).value; }

gid_t Sysdeps<GetEgid>::operator()() { return zinnia_syscall(SYSCALL_GETEGID).value; }

uid_t Sysdeps<GetUid>::operator()() { return zinnia_syscall(SYSCALL_GETUID).value; }

uid_t Sysdeps<GetEuid>::operator()() { return zinnia_syscall(SYSCALL_GETEUID).value; }

pid_t Sysdeps<GetPid>::operator()() { return zinnia_syscall(SYSCALL_GETPID).value; }

pid_t Sysdeps<GetTid>::operator()() { return zinnia_syscall(SYSCALL_GETTID).value; }

pid_t Sysdeps<GetPpid>::operator()() { return zinnia_syscall(SYSCALL_GETPPID).value; }

int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
	auto r = zinnia_syscall(SYSCALL_GETPGID, pid);
	if (r.error)
		return r.error;
	*pgid = r.value;
	return 0;
}

int Sysdeps<GetSid>::operator()(pid_t pid, pid_t *sid) {
	auto r = zinnia_syscall(SYSCALL_GETSID, pid);
	if (r.error)
		return r.error;
	*sid = r.value;
	return 0;
}

int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
	return zinnia_syscall(SYSCALL_SETPGID, pid, pgid).error;
}
int Sysdeps<SetUid>::operator()(uid_t uid) { return zinnia_syscall(SYSCALL_SETUID, uid).error; }
int Sysdeps<SetEuid>::operator()(uid_t euid) { return zinnia_syscall(SYSCALL_SETEUID, euid).error; }
int Sysdeps<SetGid>::operator()(gid_t gid) { return zinnia_syscall(SYSCALL_SETGID, gid).error; }
int Sysdeps<SetEgid>::operator()(gid_t egid) { return zinnia_syscall(SYSCALL_SETEGID, egid).error; }

int Sysdeps<GetGroups>::operator()(size_t size, gid_t *list, int *ret) {
	auto r = zinnia_syscall(SYSCALL_GETGROUPS, size, (size_t)list);
	if (r.error)
		return r.error;
	*ret = (int)r.value;
	return 0;
}

int Sysdeps<SetGroups>::operator()(size_t size, const gid_t *list) {
	return zinnia_syscall(SYSCALL_SETGROUPS, size, (size_t)list).error;
}

int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
	struct timespec req = {.tv_sec = *secs, .tv_nsec = *nanos};
	struct timespec rem = {};
	auto r = zinnia_syscall(SYSCALL_SLEEP, (size_t)&req, (size_t)&rem);
	if (r.error)
		return r.error;
	*secs = rem.tv_sec;
	*nanos = rem.tv_nsec;
	return 0;
}

void Sysdeps<Yield>::operator()() { zinnia_syscall(SYSCALL_YIELD); }

int
Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	if (ru) {
		mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported" << frg::endlog;
		return ENOSYS;
	}
again:
	auto r = zinnia_syscall(SYSCALL_WAITPID, pid, (size_t)status, flags);
	if (r.error) {
		if (r.error == EINTR)
			goto again;
		return r.error;
	}
	*ret_pid = (pid_t)r.value;
	return 0;
}

int Sysdeps<Fork>::operator()(pid_t *child) {
	auto r = zinnia_syscall(SYSCALL_FORK);
	if (r.error)
		return r.error;
	*child = r.value;
	return 0;
}

int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	return zinnia_syscall(SYSCALL_EXECVE, (size_t)path, (size_t)argv, (size_t)envp).error;
}

int Sysdeps<Pselect>::operator()(
    int num_fds,
    fd_set *read_set,
    fd_set *write_set,
    fd_set *except_set,
    const struct timespec *timeout,
    const sigset_t *sigmask,
    int *num_events
) {
	pollfd *fds = (pollfd *)malloc(num_fds * sizeof(pollfd));

	if (fds == NULL)
		return ENOMEM;

	int actual_count = 0;

	for (int fd = 0; fd < num_fds; ++fd) {
		short events = 0;
		if (read_set && FD_ISSET(fd, read_set)) {
			events |= POLLIN;
		}

		if (write_set && FD_ISSET(fd, write_set)) {
			events |= POLLOUT;
		}

		if (except_set && FD_ISSET(fd, except_set)) {
			events |= POLLPRI;
		}

		if (events) {
			fds[actual_count].fd = fd;
			fds[actual_count].events = events;
			fds[actual_count].revents = 0;
			actual_count++;
		}
	}

	int num;
	int err = sysdep<Ppoll>(fds, actual_count, timeout, sigmask, &num);

	if (err) {
		free(fds);
		return err;
	}

#define READ_SET_POLLSTUFF (POLLIN | POLLHUP | POLLERR)
#define WRITE_SET_POLLSTUFF (POLLOUT | POLLERR)
#define EXCEPT_SET_POLLSTUFF (POLLPRI)

	int return_count = 0;
	for (int fd = 0; fd < actual_count; ++fd) {
		int events = fds[fd].events;
		if ((events & POLLIN) && (fds[fd].revents & READ_SET_POLLSTUFF) == 0) {
			FD_CLR(fds[fd].fd, read_set);
			events &= ~POLLIN;
		}

		if ((events & POLLOUT) && (fds[fd].revents & WRITE_SET_POLLSTUFF) == 0) {
			FD_CLR(fds[fd].fd, write_set);
			events &= ~POLLOUT;
		}

		if ((events & POLLPRI) && (fds[fd].revents & EXCEPT_SET_POLLSTUFF) == 0) {
			FD_CLR(fds[fd].fd, except_set);
			events &= ~POLLPRI;
		}

		if (events)
			return_count++;
	}
	*num_events = return_count;
	free(fds);
	return 0;
}

int Sysdeps<GetRusage>::operator()(int scope, struct rusage *usage) {
	return zinnia_syscall(SYSCALL_GETRUSAGE, scope, (size_t)usage).error;
}

int Sysdeps<GetRlimit>::operator()(int resource, struct rlimit *limit) {
	return zinnia_syscall(SYSCALL_GETRLIMIT, resource, (size_t)limit).error;
}

int Sysdeps<SetRlimit>::operator()(int resource, const struct rlimit *limit) {
	return zinnia_syscall(SYSCALL_SETRLIMIT, resource, (size_t)limit).error;
}

int Sysdeps<GetPriority>::operator()(int which, id_t who, int *value) {
	auto r = zinnia_syscall(SYSCALL_GETPRIORITY, which, who);
	if (r.error)
		return r.error;
	*value = r.value;
	return 0;
}

int Sysdeps<SetPriority>::operator()(int which, id_t who, int prio) {
	return zinnia_syscall(SYSCALL_SETPRIORITY, which, who, prio).error;
}

int Sysdeps<GetParam>::operator()(pid_t pid, struct sched_param *param) {
	return zinnia_syscall(SYSCALL_SCHED_GETPARAM, pid, (size_t)param).error;
}

int Sysdeps<SetParam>::operator()(pid_t pid, const struct sched_param *param) {
	return zinnia_syscall(SYSCALL_SCHED_SETPARAM, pid, (size_t)param).error;
}

int Sysdeps<GetCwd>::operator()(char *buffer, size_t size) {
	return zinnia_syscall(SYSCALL_GETCWD, (size_t)buffer, size).error;
}

int Sysdeps<Chdir>::operator()(const char *path) {
	return zinnia_syscall(SYSCALL_CHDIR, (size_t)path).error;
}

int Sysdeps<Fchdir>::operator()(int fd) { return zinnia_syscall(SYSCALL_FCHDIR, fd).error; };

int Sysdeps<Chroot>::operator()(const char *path) {
	return zinnia_syscall(SYSCALL_CHROOT, (size_t)path).error;
}

int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
	return sysdep<Mkdirat>(AT_FDCWD, path, mode);
}

int Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode) {
	return zinnia_syscall(SYSCALL_MKDIRAT, dirfd, (size_t)path, mode).error;
}

int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
	return sysdep<Linkat>(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int Sysdeps<Linkat>::operator()(
    int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags
) {
	return zinnia_syscall(
	           SYSCALL_LINKAT, olddirfd, (size_t)old_path, newdirfd, (size_t)new_path, flags
	)
	    .error;
}

int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
	return sysdep<Symlinkat>(target_path, AT_FDCWD, link_path);
}

int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
	return zinnia_syscall(SYSCALL_SYMLINKAT, (size_t)target_path, dirfd, (size_t)link_path).error;
}

int Sysdeps<Rename>::operator()(const char *path, const char *new_path) {
	return sysdep<Renameat>(AT_FDCWD, path, AT_FDCWD, new_path);
}

int Sysdeps<Renameat>::operator()(
    int olddirfd, const char *old_path, int newdirfd, const char *new_path
) {
	return zinnia_syscall(SYSCALL_RENAMEAT, olddirfd, (size_t)old_path, newdirfd, (size_t)new_path)
	    .error;
}

int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result) {
	auto r = zinnia_syscall(SYSCALL_FCNTL, fd, request, va_arg(args, size_t));
	if (r.error)
		return r.error;
	*result = (int)r.value;
	return 0;
}

int Sysdeps<Ttyname>::operator()(int fd, char *buf, size_t size) {
	if (size >= __MLIBC_NAME_MAX) {
		mlibc::panicLogger() << "ttyname size too small" << frg::endlog;
		return ERANGE;
	}

	int res;
	return sysdep<Ioctl>(fd, TIOCGNAME, (void *)buf, &res);
}

void Sysdeps<Sync>::operator()() { zinnia_syscall(SYSCALL_SYNC); }

int Sysdeps<Fsync>::operator()(int fd) { return zinnia_syscall(SYSCALL_FSYNC, fd).error; }

int Sysdeps<Fdatasync>::operator()(int fd) { return zinnia_syscall(SYSCALL_FDATASYNC, fd).error; }

int Sysdeps<Chmod>::operator()(const char *pathname, mode_t mode) {
	return sysdep<Fchmodat>(AT_FDCWD, pathname, mode, 0);
}

int Sysdeps<Fchmod>::operator()(int fd, mode_t mode) {
	return zinnia_syscall(SYSCALL_FCHMOD, fd, mode).error;
}

int Sysdeps<Fchmodat>::operator()(int fd, const char *pathname, mode_t mode, int flags) {
	return zinnia_syscall(SYSCALL_FCHMODAT, fd, (size_t)pathname, mode, flags).error;
}

int Sysdeps<Utimensat>::operator()(
    int dirfd, const char *pathname, const struct timespec times[2], int flags
) {
	return zinnia_syscall(SYSCALL_UTIMENSAT, dirfd, (size_t)pathname, (size_t)times, flags).error;
}

int Sysdeps<SetSid>::operator()(pid_t *sid) {
	auto r = zinnia_syscall(SYSCALL_SETSID);
	if (r.error)
		return r.error;
	*sid = (pid_t)r.value;
	return 0;
}

int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr) {
	int ret;
	return sysdep<Ioctl>(fd, TCGETS, attr, &ret);
}

int Sysdeps<Tcsetattr>::operator()(int fd, int optional_action, const struct termios *attr) {
	int req;
	switch (optional_action) {
		case TCSANOW:
			req = TCSETS;
			break;
		case TCSADRAIN:
			req = TCSETSW;
			break;
		case TCSAFLUSH:
			req = TCSETSF;
			break;
		default:
			return EINVAL;
	}

	int ret;
	return sysdep<Ioctl>(fd, req, (void *)attr, &ret);
}

int Sysdeps<Tcgetwinsize>::operator()(int fd, struct winsize *winsz) {
	int result;
	return sysdep<Ioctl>(fd, TIOCGWINSZ, winsz, &result);
}

int Sysdeps<Tcsetwinsize>::operator()(int fd, const struct winsize *winsz) {
	int result;
	return sysdep<Ioctl>(fd, TIOCSWINSZ, const_cast<struct winsize *>(winsz), &result);
}

int Sysdeps<Pipe>::operator()(int *fds, int flags) {
	return zinnia_syscall(SYSCALL_PIPE, (size_t)fds, flags).error;
}

int Sysdeps<Ppoll>::operator()(
    struct pollfd *fds,
    nfds_t count,
    const struct timespec *timeout,
    const sigset_t *sigmask,
    int *num_events
) {
	auto r = zinnia_syscall(SYSCALL_PPOLL, (size_t)fds, count, (size_t)timeout, (size_t)sigmask);
	if (r.error)
		return r.error;
	*num_events = (int)r.value;
	return 0;
}

int Sysdeps<Poll>::operator()(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	struct timespec ts;
	ts.tv_sec = timeout / 1000;
	ts.tv_nsec = (timeout % 1000) * 1000000;

	return sysdep<Ppoll>(fds, count, timeout != -1 ? &ts : NULL, NULL, num_events);
}

int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
	auto r = zinnia_syscall(SYSCALL_IOCTL, fd, request, (size_t)arg);
	if (r.error)
		return r.error;
	if (result)
		*result = r.value;
	return 0;
}

int Sysdeps<Sigprocmask>::operator()(
    int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve
) {
	return zinnia_syscall(SYSCALL_SIGPROCMASK, how, (size_t)set, (size_t)retrieve).error;
}

extern "C" void __mlibc_restorer();

int Sysdeps<Sigaction>::operator()(
    int sig, const struct sigaction *__restrict act, struct sigaction *__restrict oact
) {
	if (act != nullptr) {
		struct sigaction modified = *act;
		modified.sa_restorer = __mlibc_restorer;
		return zinnia_syscall(SYSCALL_SIGACTION, sig, (size_t)&modified, (size_t)oact).error;
	}
	return zinnia_syscall(SYSCALL_SIGACTION, sig, 0, (size_t)oact).error;
}

int Sysdeps<Sigtimedwait>::operator()(
    const sigset_t *__restrict set,
    siginfo_t *__restrict info,
    const struct timespec *__restrict timeout,
    int *out_signal
) {
	auto r = zinnia_syscall(SYSCALL_SIGTIMEDWAIT, (size_t)set, (size_t)info, (size_t)timeout);
	if (r.error)
		return r.error;
	*out_signal = (int)r.value;
	return 0;
}

int Sysdeps<Kill>::operator()(pid_t pid, int signal) {
	return zinnia_syscall(SYSCALL_KILL, pid, signal).error;
}

int Sysdeps<GetHostname>::operator()(char *buffer, size_t bufsize) {
	struct utsname name;
	int i = sysdep<Uname>(&name);
	if (i)
		return i;
	if (bufsize >= sizeof(name.nodename))
		bufsize = sizeof(name.nodename) - 1;
	memcpy(buffer, name.nodename, bufsize);
	return 0;
}

int Sysdeps<SetHostname>::operator()(const char *buffer, size_t bufsize) {
	struct utsname name;
	int i = sysdep<Uname>(&name);
	if (i)
		return i;
	if (bufsize >= sizeof(name.nodename))
		bufsize = sizeof(name.nodename) - 1;
	memcpy(name.nodename, buffer, bufsize);
	return zinnia_syscall(SYSCALL_SETUNAME, (size_t)&name).error;
}

int Sysdeps<Mkfifoat>::operator()(int dirfd, const char *path, mode_t mode) {
	return sysdep<Mknodat>(dirfd, path, S_IFIFO | mode, 0);
}

int Sysdeps<GetEntropy>::operator()(void *buffer, size_t length) {
	return zinnia_syscall(SYSCALL_GETENTROPY, (size_t)buffer, length).error;
}

int Sysdeps<Mknodat>::operator()(int dirfd, const char *path, int mode, int dev) {
	return zinnia_syscall(SYSCALL_MKNODAT, dirfd, (size_t)path, mode, dev).error;
}

int Sysdeps<Umask>::operator()(mode_t mode, mode_t *old) {
	auto r = zinnia_syscall(SYSCALL_UMASK, mode);
	if (r.error)
		return r.error;
	*old = (mode_t)r.value;
	return 0;
}

int Sysdeps<Tgkill>::operator()(int pid, int tid, int sig) {
	return zinnia_syscall(SYSCALL_THREAD_KILL, pid, tid, sig).error;
}

int Sysdeps<Fchownat>::operator()(
    int dirfd, const char *pathname, uid_t owner, gid_t group, int flags
) {
	return zinnia_syscall(SYSCALL_FCHOWNAT, dirfd, (size_t)pathname, owner, group, flags).error;
}

int Sysdeps<Sigaltstack>::operator()(const stack_t *ss, stack_t *oss) {
	return zinnia_syscall(SYSCALL_SIGALTSTACK, (size_t)ss, (size_t)oss).error;
}

int Sysdeps<Sigsuspend>::operator()(const sigset_t *set) {
	zinnia_syscall(SYSCALL_SIGSUSPEND, (size_t)set);
	return EINTR;
}

int Sysdeps<Sigpending>::operator()(sigset_t *set) {
	return zinnia_syscall(SYSCALL_SIGPENDING, (size_t)set).error;
}

int Sysdeps<Madvise>::operator()(void *addr, size_t length, int advice) {
	return zinnia_syscall(SYSCALL_MADVISE, (size_t)addr, length, advice).error;
}

int Sysdeps<GetItimer>::operator()(int which, struct itimerval *curr_value) {
	return zinnia_syscall(SYSCALL_ITIMER_GET, which, (size_t)curr_value).error;
}

int Sysdeps<SetItimer>::operator()(
    int which, const struct itimerval *new_value, struct itimerval *old_value
) {
	return zinnia_syscall(SYSCALL_ITIMER_SET, which, (size_t)new_value, (size_t)old_value).error;
}

int Sysdeps<TimerCreate>::operator()(
    clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res
) {
	return zinnia_syscall(SYSCALL_TIMER_CREATE, (size_t)clk, (size_t)evp, (size_t)res).error;
}

int Sysdeps<TimerSettime>::operator()(
    timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old
) {
	return zinnia_syscall(SYSCALL_TIMER_SET, (size_t)t, flags, (size_t)val, (size_t)old).error;
}

int Sysdeps<TimerDelete>::operator()(timer_t t) {
	return zinnia_syscall(SYSCALL_TIMER_DELETE, (size_t)t).error;
}

int Sysdeps<Uname>::operator()(struct utsname *buf) {
	auto r = zinnia_syscall(SYSCALL_GETUNAME, (size_t)buf);
	return r.error;
}

int Sysdeps<SetResuid>::operator()(uid_t ruid, uid_t euid, uid_t suid) {
	return zinnia_syscall(SYSCALL_SETRESUID, ruid, euid, suid).error;
}

int Sysdeps<SetResgid>::operator()(gid_t rgid, gid_t egid, gid_t sgid) {
	return zinnia_syscall(SYSCALL_SETRESGID, rgid, egid, sgid).error;
}

int Sysdeps<GetResuid>::operator()(uid_t *ruid, uid_t *euid, uid_t *suid) {
	return zinnia_syscall(SYSCALL_GETRESUID, (size_t)ruid, (size_t)euid, (size_t)suid).error;
}

int Sysdeps<GetResgid>::operator()(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	return zinnia_syscall(SYSCALL_GETRESGID, (size_t)rgid, (size_t)egid, (size_t)sgid).error;
}

int Sysdeps<SetReuid>::operator()(uid_t ruid, uid_t euid) {
	return zinnia_syscall(SYSCALL_SETREUID, ruid, euid).error;
}

int Sysdeps<SetRegid>::operator()(gid_t rgid, gid_t egid) {
	return zinnia_syscall(SYSCALL_SETREGID, rgid, egid).error;
}

int Sysdeps<Unlockpt>::operator()(int fd) {
	int unlock = 0;
	if (int e = sysdep<Ioctl>(fd, TIOCSPTLCK, &unlock, NULL); e)
		return e;
	return 0;
}

int Sysdeps<ThreadGetname>::operator()(void *tcb, char *name, size_t size) {
	auto t = reinterpret_cast<Tcb *>(tcb);
	return zinnia_syscall(SYSCALL_THREAD_GETNAME, t->tid, (size_t)name, size).error;
}

int Sysdeps<ThreadSetname>::operator()(void *tcb, const char *name) {
	auto t = reinterpret_cast<Tcb *>(tcb);
	return zinnia_syscall(SYSCALL_THREAD_SETNAME, t->tid, (size_t)name).error;
}

int Sysdeps<Waitid>::operator()(idtype_t idtype, id_t id, siginfo_t *info, int options) {
	return zinnia_syscall(SYSCALL_WAITID, idtype, id, (size_t)info, options).error;
}

int Sysdeps<Ptsname>::operator()(int fd, char *buffer, size_t length) {
	int index;
	if (int e = sysdep<Ioctl>(fd, TIOCGPTN, &index, NULL); e)
		return e;
	if ((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
		return ERANGE;
	}
	return 0;
}

int Sysdeps<IfIndextoname>::operator()(unsigned int index, char *name) {
	int fd = 0;
	int r = sysdep<Socket>(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if (r)
		return r;

	struct ifreq ifr;
	ifr.ifr_ifindex = index;

	int ret = sysdep<Ioctl>(fd, SIOCGIFNAME, &ifr, NULL);
	close(fd);

	if (ret) {
		if (ret == ENODEV)
			return ENXIO;
		return ret;
	}

	strncpy(name, ifr.ifr_name, IF_NAMESIZE);

	return 0;
}

int Sysdeps<IfNametoindex>::operator()(const char *name, unsigned int *ret) {
	int fd = 0;
	int r = sysdep<Socket>(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if (r)
		return r;

	struct ifreq ifr;
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);

	r = sysdep<Ioctl>(fd, SIOCGIFINDEX, &ifr, NULL);
	close(fd);

	if (r) {
		return r;
	}

	*ret = ifr.ifr_ifindex;

	return 0;
}

} // namespace mlibc
