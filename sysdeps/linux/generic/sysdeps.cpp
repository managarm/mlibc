#include <errno.h>
#include <type_traits>

#include <mlibc-config.h>
#include <bits/ensure.h>
#include <abi-bits/fcntl.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <limits.h>
#include "cxx-syscall.hpp"

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

#define NR_read 0
#define NR_write 1
#define NR_open 2
#define NR_close 3
#define NR_stat 4
#define NR_fstat 5
#define NR_lseek 8
#define NR_mmap 9
#define NR_mprotect 10
#define NR_sigaction 13
#define NR_rt_sigprocmask 14
#define NR_ioctl 16
#define NR_pipe 22
#define NR_select 23
#define NR_nanosleep 35
#define NR_getpid 39
#define NR_socket 41
#define NR_connect 42
#define NR_sendmsg 46
#define NR_recvmsg 47
#define NR_clone 56
#define NR_fork 57
#define NR_execve 59
#define NR_exit 60
#define NR_wait4 61
#define NR_kill 62
#define NR_fcntl 72
#define NR_getcwd 79
#define NR_chdir 80
#define NR_mkdir 83
#define NR_rmdir 84
#define NR_unlink 87
#define NR_symlink 88
#define NR_readlink 89
#define NR_getuid 102
#define NR_getgid 104
#define NR_geteuid 107
#define NR_getegid 108
#define NR_rt_sigsuspend 130
#define NR_sigaltstack 131
#define NR_arch_prctl 158
#define NR_sys_futex 202
#define NR_clock_gettime 228
#define NR_exit_group 231
#define NR_tgkill 234
#define NR_newfstatat 262
#define NR_unlinkat 263
#define NR_pselect6 270
#define NR_dup3 292
#define NR_pipe2 293

#define ARCH_SET_FS	0x1002

namespace mlibc {

void sys_libc_log(const char *message) {
	size_t n = 0;
	while(message[n])
		n++;
	do_cp_syscall(NR_write, 2, message, n);
	char lf = '\n';
	do_cp_syscall(NR_write, 2, &lf, 1);
}

void sys_libc_panic() {
	__builtin_trap();
}

int sys_tcb_set(void *pointer) {
	auto ret = do_syscall(NR_arch_prctl, ARCH_SET_FS, pointer);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_anon_allocate(size_t size, void **pointer) {
	return sys_vm_map(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0, pointer);
}
int sys_anon_free(void *pointer, size_t size) {
	return sys_vm_unmap(pointer, size);
}

int sys_open(const char *path, int flags, int *fd) {
        // TODO: pass mode in sys_open() sysdep
	auto ret = do_cp_syscall(NR_open, path, flags, 0666);
	if(int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_close(int fd) {
	auto ret = do_cp_syscall(NR_close, fd);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
	auto ret = do_cp_syscall(NR_dup3, fd, newfd, flags);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_read(int fd, void *buffer, size_t size, ssize_t *bytes_read) {
	auto ret = do_cp_syscall(NR_read, fd, buffer, size);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_write(int fd, const void *buffer, size_t size, ssize_t *bytes_written) {
	auto ret = do_cp_syscall(NR_write, fd, buffer, size);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_written = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	auto ret = do_syscall(NR_lseek, fd, offset, whence);
	if(int e = sc_error(ret); e)
		return e;
	*new_offset = sc_int_result<off_t>(ret);
	return 0;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags,
		int fd, off_t offset, void **window) {
	auto ret = do_syscall(NR_mmap, hint, size, prot, flags, fd, offset);
	// TODO: musl fixes up EPERM errors from the kernel.
	if(int e = sc_error(ret); e)
		return e;
	*window = sc_ptr_result<void>(ret);
	return 0;
}
int sys_vm_unmap(void *pointer, size_t size) STUB_ONLY

// All remaining functions are disabled in ldso.
#ifndef MLIBC_BUILDING_RTDL

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	struct timespec tp = {};
	auto ret = do_syscall(NR_clock_gettime, clock, &tp);
	if (int e = sc_error(ret); e)
		return e;
	*secs = tp.tv_sec;
	*nanos = tp.tv_nsec;
	return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	if (fsfdt == fsfd_target::path)
		fd = AT_FDCWD;
	else if (fsfdt == fsfd_target::fd)
		flags |= AT_EMPTY_PATH;
	else
		__ensure(fsfdt == fsfd_target::fd_path);

	auto ret = do_cp_syscall(NR_newfstatat, fd, path, statbuf, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

extern "C" void __mlibc_signal_restore(void);

int sys_sigaction(int signum, const struct sigaction *act,
                struct sigaction *oldact) {
	struct ksigaction {
		void (*handler)(int);
		unsigned long flags;
		void (*restorer)(void);
		sigset_t mask;
	};

	struct ksigaction kernel_act, kernel_oldact;
	if (act) {
		kernel_act.handler = act->sa_handler;
		kernel_act.flags = act->sa_flags | SA_RESTORER;
		kernel_act.restorer = __mlibc_signal_restore;
		kernel_act.mask = act->sa_mask;
	}
        auto ret = do_syscall(NR_sigaction, signum, act ?
			&kernel_act : NULL, oldact ?
			&kernel_oldact : NULL, sizeof(sigset_t));
        if (int e = sc_error(ret); e)
                return e;

	if (oldact) {
		oldact->sa_handler = kernel_oldact.handler;
		oldact->sa_flags = kernel_oldact.flags;
		oldact->sa_restorer = kernel_oldact.restorer;
		oldact->sa_mask = kernel_oldact.mask;
	}
        return 0;
}

int sys_socket(int domain, int type, int protocol, int *fd) {
        auto ret = do_syscall(NR_socket, domain, type, protocol);
        if (int e = sc_error(ret); e)
                return e;
        *fd = sc_int_result<int>(ret);
        return 0;
}

int sys_msg_send(int sockfd, const struct msghdr *msg, int flags, ssize_t *length) {
        auto ret = do_cp_syscall(NR_sendmsg, sockfd, msg, flags);
        if (int e = sc_error(ret); e)
                return e;
        *length = sc_int_result<ssize_t>(ret);
        return 0;
}

int sys_msg_recv(int sockfd, struct msghdr *msg, int flags, ssize_t *length) {
        auto ret = do_cp_syscall(NR_recvmsg, sockfd, msg, flags);
        if (int e = sc_error(ret); e)
                return e;
        *length = sc_int_result<ssize_t>(ret);
        return 0;
}

int sys_fcntl(int fd, int cmd, va_list args, int *result) {
        auto arg = va_arg(args, unsigned long);
        // TODO: the api for linux differs for each command so fcntl()s might fail with -EINVAL
        // we should implement all the different fcntl()s
	// TODO(geert): only some fcntl()s can fail with -EINTR, making do_cp_syscall useless
	// on most fcntls(). Another reason to handle different fcntl()s seperately.
        auto ret = do_cp_syscall(NR_fcntl, fd, cmd, arg);
        if (int e = sc_error(ret); e)
                return e;
        *result = sc_int_result<int>(ret);
        return 0;
}

int sys_getcwd(char *buf, size_t size) {
	auto ret = do_syscall(NR_getcwd, buf, size);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int sys_unlinkat(int dfd, const char *path, int flags) {
	auto ret = do_syscall(NR_unlinkat, dfd, path, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_sleep(time_t *secs, long *nanos) {
	struct timespec req = {
		.tv_sec = *secs,
		.tv_nsec = *nanos
	};
	struct timespec rem = {0};

	auto ret = do_cp_syscall(NR_nanosleep, &req, &rem);
        if (int e = sc_error(ret); e)
                return e;

	*secs = rem.tv_sec;
	*nanos = rem.tv_nsec;
	return 0;
}

#ifdef __MLIBC_POSIX_OPTION

#include <sys/ioctl.h>
#include <sched.h>

int sys_isatty(int fd) {
        struct winsize ws;
        auto ret = do_syscall(NR_ioctl, fd, TIOCGWINSZ, &ws);
        if (int e = sc_error(ret); e)
                return e;
        auto res = sc_int_result<unsigned long>(ret);
        if(!res) return 0;
        return 1;
}

int sys_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
        auto ret = do_cp_syscall(NR_connect, sockfd, addr, addrlen);
        if (int e = sc_error(ret); e)
                return e;
        return 0;
}

int sys_pselect(int nfds, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, struct timeval *timeout, const sigset_t sigmask, int *num_events) {
        // The Linux kernel really wants 7 arguments, even tho this is not supported
        // To fix that issue, they use a struct as the last argument.
        // See the man page of pselect and the glibc source code
        struct {
                sigset_t ss;
                size_t ss_len;
        } data;
        data.ss = (uintptr_t)sigmask;
        data.ss_len = NSIG / 8;

        auto ret = do_cp_syscall(NR_pselect6, nfds, readfds, writefds,
                        exceptfds, timeout, &data);
        if (int e = sc_error(ret); e)
                return e;
        *num_events = sc_int_result<int>(ret);
        return 0;
}

int sys_pipe(int *fds, int flags) {
        if(flags) {
                auto ret = do_syscall(NR_pipe2, fds, flags);
                if (int e = sc_error(ret); e)
                        return e;
                return 0;
        } else {
                auto ret = do_syscall(NR_pipe, fds);
                if (int e = sc_error(ret); e)
                        return e;
                return 0;
        }
}

int sys_fork(pid_t *child) {
        auto ret = do_syscall(NR_fork);
        if (int e = sc_error(ret); e)
                return e;
        *child = sc_int_result<int>(ret);
        return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid) {
        auto ret = do_syscall(NR_wait4, pid, status, flags, 0);
        if (int e = sc_error(ret); e)
                return e;
        *ret_pid = sc_int_result<int>(ret);
        return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
        auto ret = do_syscall(NR_execve, path, argv, envp);
        if (int e = sc_error(ret); e)
                return e;
        return 0;
}

int sys_sigprocmask(int how, const sigset_t *set, sigset_t *old) {
        auto ret = do_syscall(NR_rt_sigprocmask, how, set, old, NSIG / 8);
        if (int e = sc_error(ret); e)
                return e;
	return 0;
}

int sys_clone(void *entry, void *user_arg, void *tcb, pid_t *pid_out) {
        void *stack = prepare_stack(entry, user_arg);

	unsigned long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND
		| CLONE_THREAD | CLONE_SYSVSEM | CLONE_SETTLS | CLONE_SETTLS
		| CLONE_PARENT_SETTID;

	auto ret = __mlibc_spawn_thread(flags, stack, pid_out, NULL, tcb);
	if (ret < 0)
		return ret;

        return 0;
}

extern "C" const char __mlibc_syscall_begin[1];
extern "C" const char __mlibc_syscall_end[1];

int sys_before_cancellable_syscall(ucontext_t *uct) {
	auto pc = reinterpret_cast<void*>(uct->uc_mcontext.rip);
	if (pc < __mlibc_syscall_begin || pc > __mlibc_syscall_end)
		return 0;
	return 1;
}

int sys_tgkill(int tgid, int tid, int sig) {
	auto ret = do_syscall(NR_tgkill, tgid, tid, sig);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#endif // __MLIBC_POSIX_OPTION

pid_t sys_getpid() {
	auto ret = do_syscall(NR_getpid);
	// getpid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

uid_t sys_getuid() {
	auto ret = do_syscall(NR_getuid);
	// getuid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

uid_t sys_geteuid() {
	auto ret = do_syscall(NR_geteuid);
	// geteuid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

gid_t sys_getgid() {
	auto ret = do_syscall(NR_getgid);
	// getgid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

gid_t sys_getegid() {
	auto ret = do_syscall(NR_getegid);
	// getegid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

int sys_kill(int pid, int sig) {
	auto ret = do_syscall(NR_kill, pid, sig);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
	auto ret = do_syscall(NR_mprotect, pointer, size, prot);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

void sys_thread_exit() {
	do_syscall(NR_exit, 0);
	__builtin_trap();
}

void sys_exit(int status) {
	do_syscall(NR_exit_group, status);
	__builtin_trap();
}

#endif // MLIBC_BUILDING_RTDL

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	auto ret = do_cp_syscall(NR_sys_futex, pointer, FUTEX_WAIT, expected, time);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_futex_wake(int *pointer) {
	auto ret = do_syscall(NR_sys_futex, pointer, FUTEX_WAKE, INT_MAX);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_sigsuspend(const sigset_t *set) {
	auto ret = do_syscall(NR_rt_sigsuspend, set, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_sigaltstack(const stack_t *ss, stack_t *oss) {
	auto ret = do_syscall(NR_sigaltstack, ss, oss);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_mkdir(const char *path) {
	auto ret = do_syscall(NR_mkdir, path, S_IRWXU | S_IRWXG | S_IRWXO);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_symlink(const char *target_path, const char *link_path) {
	auto ret = do_syscall(NR_symlink, target_path, link_path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_chdir(const char *path) {
	auto ret = do_syscall(NR_chdir, path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_rmdir(const char *path) {
	auto ret = do_syscall(NR_rmdir, path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_readlink(const char *path, void *buf, size_t bufsiz, ssize_t *len) {
	auto ret = do_syscall(NR_readlink, path, buf, bufsiz);
	if (int e = sc_error(ret); e)
		return e;
	*len = sc_int_result<ssize_t>(ret);
	return 0;
}

} // namespace mlibc
