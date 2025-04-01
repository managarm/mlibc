#include <abi-bits/limits.h>
#include <abi-bits/pid_t.h>
#include <asm/ioctls.h>
#include <errno.h>
#include <frg/logging.hpp>
#include <menix/power.hpp>
#include <menix/syscall.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/tcb.hpp>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

namespace mlibc {

[[noreturn]] void sys_exit(int status) {
	menix_syscall(SYSCALL_EXIT, status);
	__builtin_unreachable();
}

[[noreturn]] void sys_thread_exit() {
	menix_syscall(SYSCALL_THREAD_EXIT, 0);
	__builtin_unreachable();
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	struct timespec ts;
	auto r = menix_syscall(SYSCALL_CLOCK_GET, clock, (size_t)&ts);
	if (r.error)
		return r.error;
	*secs = ts.tv_sec;
	*nanos = ts.tv_nsec;
	return 0;
}

int sys_clock_getres(int clock, time_t *secs, long *nanos) {
	struct timespec ts;
	auto r = menix_syscall(SYSCALL_CLOCK_GETRES, clock, (size_t)&ts);
	if (r.error)
		return r.error;
	*secs = ts.tv_sec;
	*nanos = ts.tv_nsec;
	return 0;
}

int sys_flock(int fd, int options) {
	auto r = menix_syscall(SYSCALL_FLOCK, fd, options);
	return r.error;
}

int sys_open_dir(const char *path, int *handle) { return sys_open(path, O_DIRECTORY, 0, handle); }

int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	auto r = menix_syscall(SYSCALL_GETDENTS, handle, (size_t)buffer, (size_t)max_size);
	if (r.error) {
		return r.error;
	}
	*bytes_read = r.value;
	return 0;
}

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	auto r = menix_syscall(SYSCALL_WRITE, fd, (size_t)buf, count);
	if (r.error)
		return r.error;
	*bytes_written = r.value;
	return 0;
}

int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	auto r = menix_syscall(SYSCALL_PREAD, fd, (size_t)buf, n, off);
	if (r.error)
		return r.error;
	*bytes_read = r.value;
	return 0;
}

int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	auto r = menix_syscall(SYSCALL_PWRITE, fd, (size_t)buf, n, off);
	if (r.error)
		return r.error;
	*bytes_read = r.value;
	return 0;
}

int sys_access(const char *path, int mode) { return sys_faccessat(AT_FDCWD, path, mode, 0); }

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
	return menix_syscall(SYSCALL_FACCESSAT, dirfd, (size_t)pathname, mode, flags).error;
}

int sys_dup(int fd, int flags, int *newfd) {
	auto r = menix_syscall(SYSCALL_DUP, fd, flags);
	if (r.error)
		return r.error;
	*newfd = r.value;
	return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
	// dup2 is roughly equal to dup3.
	return menix_syscall(SYSCALL_DUP3, fd, newfd, flags).error;
}

int sys_isatty(int fd) {
	struct winsize ws;
	// If we're a TTY then this call will succeed.
	if (!sys_ioctl(fd, TIOCGWINSZ, &ws, 0))
		return 0;

	return ENOTTY;
}

int sys_statvfs(const char *path, struct statvfs *out) {
	return menix_syscall(SYSCALL_STATVFS, (size_t)path, (size_t)out).error;
}

int sys_fstatvfs(int fd, struct statvfs *out) {
	return menix_syscall(SYSCALL_FSTATVFS, fd, (size_t)out).error;
}

int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length) {
	return sys_readlinkat(AT_FDCWD, path, buffer, max_size, length);
}

int sys_readlinkat(int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length) {
	auto r =
	    menix_syscall(SYSCALL_READLINKAT, dirfd, (size_t)path, (size_t)buffer, (size_t)max_size);
	if (r.error)
		return r.error;
	*length = r.value;
	return 0;
}

int sys_rmdir(const char *path) {
	return menix_syscall(SYSCALL_RMDIRAT, AT_FDCWD, (size_t)path).error;
}

int sys_ftruncate(int fd, size_t size) { return menix_syscall(SYSCALL_FTRUNCATE, fd, size).error; }

int sys_fallocate(int fd, off_t offset, size_t size) {
	return menix_syscall(SYSCALL_FALLOCATE, fd, offset, size).error;
}

int sys_unlinkat(int fd, const char *path, int flags) {
	return menix_syscall(SYSCALL_UNLINKAT, fd, (size_t)path, flags).error;
}

int sys_socket(int family, int type, int protocol, int *fd) {
	auto r = menix_syscall(SYSCALL_SOCKET, family, type, protocol);
	if (r.error)
		return r.error;
	*fd = (int)r.value;
	return 0;
}

int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length) {
	auto r = menix_syscall(SYSCALL_SENDMSG, fd, (size_t)hdr, flags);
	if (r.error)
		return r.error;
	*length = (ssize_t)r.value;
	return 0;
}

ssize_t sys_sendto(
    int fd,
    const void *buffer,
    size_t size,
    int flags,
    const struct sockaddr *sock_addr,
    socklen_t addr_length,
    ssize_t *length
) {
	auto r = menix_syscall(
	    SYSCALL_SENDTO, fd, (size_t)buffer, size, flags, (size_t)sock_addr, addr_length
	);
	if (r.error)
		return r.error;
	*length = (ssize_t)r.value;
	return 0;
}

int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length) {
	auto r = menix_syscall(SYSCALL_RECVMSG, fd, (size_t)hdr, flags);
	if (r.error)
		return r.error;
	*length = (ssize_t)r.value;
	return 0;
}

ssize_t sys_recvfrom(
    int fd,
    void *buffer,
    size_t size,
    int flags,
    struct sockaddr *sock_addr,
    socklen_t *addr_length,
    ssize_t *length
) {
	auto r = menix_syscall(
	    SYSCALL_RECVFROM, fd, (size_t)buffer, size, flags, (size_t)sock_addr, (size_t)addr_length
	);
	if (r.error)
		return r.error;
	*length = (ssize_t)r.value;
	return 0;
}

int sys_listen(int fd, int backlog) { return menix_syscall(SYSCALL_LISTEN, fd, backlog).error; }

gid_t sys_getgid() { return menix_syscall(SYSCALL_GETGID).value; }
gid_t sys_getegid() { return menix_syscall(SYSCALL_GETEGID).value; }
uid_t sys_getuid() { return menix_syscall(SYSCALL_GETUID).value; }
uid_t sys_geteuid() { return menix_syscall(SYSCALL_GETEUID).value; }
pid_t sys_getpid() { return menix_syscall(SYSCALL_GETPID).value; }
pid_t sys_gettid() { return menix_syscall(SYSCALL_GETTID).value; }
pid_t sys_getppid() { return menix_syscall(SYSCALL_GETPPID).value; }

int sys_getpgid(pid_t pid, pid_t *pgid) {
	auto r = menix_syscall(SYSCALL_GETPGID, pid);
	if (r.error)
		return r.error;
	*pgid = r.value;
	return 0;
}

int sys_getsid(pid_t pid, pid_t *sid) {
	auto r = menix_syscall(SYSCALL_GETSID, pid);
	if (r.error)
		return r.error;
	*sid = r.value;
	return 0;
}

int sys_setpgid(pid_t pid, pid_t pgid) { return menix_syscall(SYSCALL_SETPGID, pid, pgid).error; }
int sys_setuid(uid_t uid) { return menix_syscall(SYSCALL_SETUID, uid).error; }
int sys_seteuid(uid_t euid) { return menix_syscall(SYSCALL_SETEUID, euid).error; }
int sys_setgid(gid_t gid) { return menix_syscall(SYSCALL_SETGID, gid).error; }
int sys_setegid(gid_t egid) { return menix_syscall(SYSCALL_SETEGID, egid).error; }

int sys_getgroups(size_t size, gid_t *list, int *ret) {
	auto r = menix_syscall(SYSCALL_GETGROUPS, size, (size_t)list);
	if (r.error)
		return r.error;
	*ret = (int)r.value;
	return 0;
}

int sys_setgroups(size_t size, const gid_t *list) {
	return menix_syscall(SYSCALL_SETGROUPS, size, (size_t)list).error;
}

int sys_sleep(time_t *secs, long *nanos) {
	struct timespec req = {.tv_sec = *secs, .tv_nsec = *nanos};
	struct timespec rem = {};
	auto r = menix_syscall(SYSCALL_SLEEP, (size_t)&req, (size_t)&rem);
	if (r.error)
		return r.error;
	*secs = rem.tv_sec;
	*nanos = rem.tv_nsec;
	return 0;
}

void sys_yield() { menix_syscall(SYSCALL_YIELD); }

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	if (ru) {
		mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported" << frg::endlog;
		return ENOSYS;
	}
again:
	auto r = menix_syscall(SYSCALL_WAITPID, pid, (size_t)status, flags);
	if (r.error) {
		if (r.error == EINTR)
			goto again;
		return r.error;
	}
	*ret_pid = (pid_t)r.value;
	return 0;
}

int sys_fork(pid_t *child) {
	auto r = menix_syscall(SYSCALL_FORK);
	if (r.error)
		return r.error;
	*child = r.value;
	return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	return menix_syscall(SYSCALL_EXECVE, (size_t)path, (size_t)argv, (size_t)envp).error;
}

int sys_pselect(
    int num_fds,
    fd_set *read_set,
    fd_set *write_set,
    fd_set *except_set,
    const struct timespec *timeout,
    const sigset_t *sigmask,
    int *num_events
) {
	auto r = menix_syscall(
	    SYSCALL_PSELECT,
	    num_fds,
	    (size_t)read_set,
	    (size_t)write_set,
	    (size_t)except_set,
	    (size_t)timeout,
	    (size_t)sigmask
	);
	if (r.error)
		return r.error;
	*num_events = (int)r.value;
	return 0;
}

int sys_getrusage(int scope, struct rusage *usage) {
	return menix_syscall(SYSCALL_GETRUSAGE, scope, (size_t)usage).error;
}

int sys_getrlimit(int resource, struct rlimit *limit) {
	return menix_syscall(SYSCALL_GETRLIMIT, resource, (size_t)limit).error;
}

int sys_setrlimit(int resource, const struct rlimit *limit) {
	return menix_syscall(SYSCALL_SETRLIMIT, resource, (size_t)limit).error;
}

int sys_getpriority(int which, id_t who, int *value) {
	auto r = menix_syscall(SYSCALL_GETPRIORITY, which, who);
	if (r.error)
		return r.error;
	*value = r.value;
	return 0;
}

int sys_setpriority(int which, id_t who, int prio) {
	return menix_syscall(SYSCALL_GETPRIORITY, which, who, prio).error;
}

int sys_getparam(pid_t pid, struct sched_param *param) {
	return menix_syscall(SYSCALL_SCHED_GETPARAM, pid, (size_t)param).error;
}

int sys_setparam(pid_t pid, const struct sched_param *param) {
	return menix_syscall(SYSCALL_SCHED_GETPARAM, pid, (size_t)param).error;
}

int sys_getcwd(char *buffer, size_t size) {
	return menix_syscall(SYSCALL_GETCWD, (size_t)buffer, size).error;
}

int sys_chdir(const char *path) { return menix_syscall(SYSCALL_CHDIR, (size_t)path).error; }

int sys_fchdir(int fd) { return menix_syscall(SYSCALL_FCHDIR, fd).error; };

int sys_chroot(const char *path) { return menix_syscall(SYSCALL_CHROOT, (size_t)path).error; }

int sys_mkdir(const char *path, mode_t mode) { return sys_mkdirat(AT_FDCWD, path, mode); }

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	return menix_syscall(SYSCALL_MKDIRAT, dirfd, (size_t)path, mode).error;
}

int sys_link(const char *old_path, const char *new_path) {
	return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	return menix_syscall(
	           SYSCALL_LINKAT, olddirfd, (size_t)old_path, newdirfd, (size_t)new_path, flags
	)
	    .error;
}

int sys_symlink(const char *target_path, const char *link_path) {
	return sys_symlinkat(target_path, AT_FDCWD, link_path);
}

int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
	return menix_syscall(SYSCALL_SYMLINKAT, (size_t)target_path, dirfd, (size_t)link_path).error;
}

int sys_rename(const char *path, const char *new_path) {
	return sys_renameat(AT_FDCWD, path, AT_FDCWD, new_path);
}

int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
	return menix_syscall(SYSCALL_RENAMEAT, olddirfd, (size_t)old_path, newdirfd, (size_t)new_path)
	    .error;
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
	auto r = menix_syscall(SYSCALL_FCNTL, fd, request, va_arg(args, size_t));
	if (r.error)
		return r.error;
	*result = (int)r.value;
	return 0;
}

int sys_ttyname(int fd, char *buf, size_t size) {
	if (size >= NAME_MAX) {
		mlibc::panicLogger() << "ttyname size too small" << frg::endlog;
		__builtin_unreachable();
	}

	int res;
	return sys_ioctl(fd, TIOCGNAME, (void *)buf, &res);
}

void sys_sync() { menix_syscall(SYSCALL_SYNC); }

int sys_fsync(int fd) { return menix_syscall(SYSCALL_FSYNC, fd).error; }

int sys_fdatasync(int fd) { return menix_syscall(SYSCALL_FDATASYNC, fd).error; }

int sys_chmod(const char *pathname, mode_t mode) {
	return sys_fchmodat(AT_FDCWD, pathname, mode, 0);
}

int sys_fchmod(int fd, mode_t mode) { return menix_syscall(SYSCALL_FCHMOD, fd, mode).error; }

int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
	return menix_syscall(SYSCALL_FCHMOD, fd, (size_t)pathname, mode, flags).error;
}

int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	return menix_syscall(SYSCALL_UTIMENSAT, dirfd, (size_t)pathname, (size_t)times, flags).error;
}

int sys_setsid(pid_t *sid) {
	auto r = menix_syscall(SYSCALL_SETSID);
	if (r.error)
		return r.error;
	*sid = (pid_t)r.value;
	return 0;
}

int sys_tcgetattr(int fd, struct termios *attr) {
	int ret;
	return sys_ioctl(fd, TCGETS, attr, &ret);
}

int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
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
	return sys_ioctl(fd, req, (void *)attr, &ret);
}

int sys_pipe(int *fds, int flags) { return menix_syscall(SYSCALL_PIPE, (size_t)fds, flags).error; }

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
	auto r = menix_syscall(SYSCALL_SOCKETPAIR, domain, type_and_flags, proto);
	if (r.error)
		return r.error;
	*fds = (int)r.value;
	return 0;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	auto r = menix_syscall(SYSCALL_POLL, (size_t)fds, count, timeout);
	if (r.error)
		return r.error;
	*num_events = (int)r.value;
	return 0;
}

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
	auto r = menix_syscall(SYSCALL_IOCTL, fd, request, (size_t)arg);
	if (r.error)
		return r.error;
	*result = r.value;
	return 0;
}

int
sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
	return menix_syscall(SYSCALL_GETSOCKOPT, fd, layer, number, (size_t)buffer, (size_t)size).error;
}

int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
	return menix_syscall(SYSCALL_SETSOCKOPT, fd, layer, number, (size_t)buffer, size).error;
}

int sys_shutdown(int sockfd, int how) { return menix_syscall(SYSCALL_SHUTDOWN, sockfd, how).error; }

int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	return menix_syscall(SYSCALL_SIGPROCMASK, how, (size_t)set, (size_t)retrieve).error;
}

int
sys_sigaction(int sig, const struct sigaction *__restrict act, struct sigaction *__restrict oact) {
	return menix_syscall(SYSCALL_SIGACTION, sig, (size_t)act, (size_t)oact).error;
}

int sys_sigtimedwait(
    const sigset_t *__restrict set,
    siginfo_t *__restrict info,
    const struct timespec *__restrict timeout,
    int *out_signal
) {
	auto r = menix_syscall(SYSCALL_SIGTIMEDWAIT, (size_t)set, (size_t)info, (size_t)timeout);
	if (r.error)
		return r.error;
	*out_signal = (int)r.value;
	return 0;
}

int sys_kill(int pid, int signal) { return menix_syscall(SYSCALL_KILL, pid, signal).error; }

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	return menix_syscall(
	           SYSCALL_ACCEPT, fd, (size_t)newfd, (size_t)addr_ptr, (size_t)addr_length, flags
	)
	    .error;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	return menix_syscall(SYSCALL_BIND, fd, (size_t)addr_ptr, addr_length).error;
}

int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	return menix_syscall(SYSCALL_CONNECT, fd, (size_t)addr_ptr, addr_length).error;
}

int sys_sockname(
    int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length
) {
	auto r = menix_syscall(SYSCALL_GETSOCKNAME, fd, (size_t)addr_ptr, (size_t)&max_addr_length);
	if (r.error)
		return r.error;
	*actual_length = max_addr_length;
	return 0;
}

int sys_peername(
    int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length
) {
	auto r = menix_syscall(SYSCALL_GETPEERNAME, fd, (size_t)addr_ptr, (size_t)&max_addr_length);
	if (r.error)
		return r.error;
	*actual_length = (socklen_t)max_addr_length;
	return 0;
}

int sys_gethostname(char *buffer, size_t bufsize) {
	return menix_syscall(SYSCALL_GETHOSTNAME, (size_t)buffer, bufsize).error;
}

int sys_sethostname(const char *buffer, size_t bufsize) {
	return menix_syscall(SYSCALL_SETHOSTNAME, (size_t)buffer, bufsize).error;
}

int sys_mkfifoat(int dirfd, const char *path, mode_t mode) {
	return sys_mknodat(dirfd, path, S_IFIFO | mode, 0);
}

int sys_getentropy(void *buffer, size_t length) {
	return menix_syscall(SYSCALL_GETENTROPY, (size_t)buffer, length).error;
}

int sys_mknodat(int dirfd, const char *path, int mode, int dev) {
	return menix_syscall(SYSCALL_MKNODAT, dirfd, (size_t)path, mode, dev).error;
}

int sys_umask(mode_t mode, mode_t *old) {
	auto r = menix_syscall(SYSCALL_UMASK, mode);
	if (r.error)
		return r.error;
	*old = (mode_t)r.value;
	return 0;
}

int sys_tgkill(int pid, int tid, int sig) {
	return menix_syscall(SYSCALL_THREAD_KILL, pid, tid, sig).error;
}

int sys_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
	return menix_syscall(SYSCALL_FCHOWNAT, dirfd, (size_t)pathname, owner, group, flags).error;
}

int sys_sigaltstack(const stack_t *ss, stack_t *oss) {
	return menix_syscall(SYSCALL_SIGALTSTACK, (size_t)ss, (size_t)oss).error;
}

int sys_sigsuspend(const sigset_t *set) {
	menix_syscall(SYSCALL_SIGSUSPEND, (size_t)set);
	return EINTR;
}

int sys_sigpending(sigset_t *set) { return menix_syscall(SYSCALL_SIGPENDING, (size_t)set).error; }

int sys_madvise(void *addr, size_t length, int advice) {
	return menix_syscall(SYSCALL_MADVISE, (size_t)addr, length, advice).error;
}

int sys_getitimer(int which, struct itimerval *curr_value) {
	return menix_syscall(SYSCALL_ITIMER_GET, which, (size_t)curr_value).error;
}

int sys_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
	return menix_syscall(SYSCALL_ITIMER_SET, which, (size_t)new_value, (size_t)old_value).error;
}

int sys_timer_create(clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res) {
	return menix_syscall(SYSCALL_TIMER_CREATE, (size_t)clk, (size_t)evp, (size_t)res).error;
}

int sys_timer_settime(
    timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old
) {
	return menix_syscall(SYSCALL_TIMER_SET, (size_t)t, flags, (size_t)val, (size_t)old).error;
}

int sys_timer_delete(timer_t t) { return menix_syscall(SYSCALL_TIMER_DELETE, (size_t)t).error; }

int sys_uname(struct utsname *buf) {
	auto r = menix_syscall(SYSCALL_UNAME, (size_t)buf);
	return r.error;
}

int sys_setresuid(uid_t ruid, uid_t euid, uid_t suid) {
	return menix_syscall(SYSCALL_SETRESUID, ruid, euid, suid).error;
}

int sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid) {
	return menix_syscall(SYSCALL_SETRESGID, rgid, egid, sgid).error;
}

int sys_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) {
	return menix_syscall(SYSCALL_GETRESUID, (size_t)ruid, (size_t)euid, (size_t)suid).error;
}

int sys_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	return menix_syscall(SYSCALL_GETRESGID, (size_t)rgid, (size_t)egid, (size_t)sgid).error;
}

int sys_setreuid(uid_t ruid, uid_t euid) {
	return menix_syscall(SYSCALL_SETREUID, ruid, euid).error;
}

int sys_setregid(gid_t rgid, gid_t egid) {
	return menix_syscall(SYSCALL_SETREGID, rgid, egid).error;
}

int sys_unlockpt(int fd) {
	int unlock = 0;
	if (int e = sys_ioctl(fd, TIOCSPTLCK, &unlock, NULL); e)
		return e;
	return 0;
}

int sys_thread_getname(void *tcb, char *name, size_t size) {
	auto t = reinterpret_cast<Tcb *>(tcb);
	return menix_syscall(SYSCALL_THREAD_GETNAME, t->tid, (size_t)name, size).error;
}

int sys_thread_setname(void *tcb, const char *name) {
	auto t = reinterpret_cast<Tcb *>(tcb);
	return menix_syscall(SYSCALL_THREAD_SETNAME, t->tid, (size_t)name).error;
}

int sys_waitid(idtype_t idtype, id_t id, siginfo_t *info, int options) {
	return menix_syscall(SYSCALL_WAITID, idtype, id, (size_t)info, options).error;
}

int sys_ptsname(int fd, char *buffer, size_t length) {
	int index;
	if (int e = sys_ioctl(fd, TIOCGPTN, &index, NULL); e)
		return e;
	if ((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
		return ERANGE;
	}
	return 0;
}

int sys_if_indextoname(unsigned int index, char *name) {
	int fd = 0;
	int r = sys_socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if (r)
		return r;

	struct ifreq ifr;
	ifr.ifr_ifindex = index;

	int ret = sys_ioctl(fd, SIOCGIFNAME, &ifr, NULL);
	close(fd);

	if (ret) {
		if (ret == ENODEV)
			return ENXIO;
		return ret;
	}

	strncpy(name, ifr.ifr_name, IF_NAMESIZE);

	return 0;
}

int sys_if_nametoindex(const char *name, unsigned int *ret) {
	int fd = 0;
	int r = sys_socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if (r)
		return r;

	struct ifreq ifr;
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);

	r = sys_ioctl(fd, SIOCGIFINDEX, &ifr, NULL);
	close(fd);

	if (r) {
		return r;
	}

	*ret = ifr.ifr_ifindex;

	return 0;
}

} // namespace mlibc
