#include <errno.h>
#include <type_traits>

#include <mlibc-config.h>
#include <bits/ensure.h>
#include <abi-bits/fcntl.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <limits.h>
#include <sys/syscall.h>
#include "cxx-syscall.hpp"

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }
#define UNUSED(x) (void)(x);

#ifndef MLIBC_BUILDING_RTDL
extern "C" long __do_syscall_ret(unsigned long ret) {
	if(ret > -4096UL) {
		errno = -ret;
		return -1;
	}
	return ret;
}
#endif

namespace mlibc {

void sys_libc_log(const char *message) {
	size_t n = 0;
	while(message[n])
		n++;
	do_syscall(SYS_write, 2, message, n);
	char lf = '\n';
	do_syscall(SYS_write, 2, &lf, 1);
}

void sys_libc_panic() {
	__builtin_trap();
}

int sys_tcb_set(void *pointer) {
#if defined(__x86_64__)
	auto ret = do_syscall(SYS_arch_prctl, 0x1002 /* ARCH_SET_FS */, pointer);
	if(int e = sc_error(ret); e)
		return e;
#elif defined(__riscv)
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile ("mv tp, %0" :: "r"(thread_data));
#else
#error "Missing architecture specific code."
#endif
	return 0;
}

int sys_anon_allocate(size_t size, void **pointer) {
	return sys_vm_map(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0, pointer);
}
int sys_anon_free(void *pointer, size_t size) {
	return sys_vm_unmap(pointer, size);
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	auto ret = do_cp_syscall(SYS_openat, AT_FDCWD, path, flags, mode);
	if(int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_close(int fd) {
	auto ret = do_cp_syscall(SYS_close, fd);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
	auto ret = do_cp_syscall(SYS_dup3, fd, newfd, flags);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_read(int fd, void *buffer, size_t size, ssize_t *bytes_read) {
	auto ret = do_cp_syscall(SYS_read, fd, buffer, size);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_write(int fd, const void *buffer, size_t size, ssize_t *bytes_written) {
	auto ret = do_cp_syscall(SYS_write, fd, buffer, size);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_written = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	auto ret = do_syscall(SYS_lseek, fd, offset, whence);
	if(int e = sc_error(ret); e)
		return e;
	*new_offset = sc_int_result<off_t>(ret);
	return 0;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags,
		int fd, off_t offset, void **window) {
	auto ret = do_syscall(SYS_mmap, hint, size, prot, flags, fd, offset);
	// TODO: musl fixes up EPERM errors from the kernel.
	if(int e = sc_error(ret); e)
		return e;
	*window = sc_ptr_result<void>(ret);
	return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
	auto ret = do_syscall(NR_munmap, pointer, size);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

// All remaining functions are disabled in ldso.
#ifndef MLIBC_BUILDING_RTDL

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	struct timespec tp = {};
	auto ret = do_syscall(SYS_clock_gettime, clock, &tp);
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

	auto ret = do_cp_syscall(SYS_newfstatat, fd, path, statbuf, flags);
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
        auto ret = do_syscall(SYS_rt_sigaction, signum, act ?
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
        auto ret = do_syscall(SYS_socket, domain, type, protocol);
        if (int e = sc_error(ret); e)
                return e;
        *fd = sc_int_result<int>(ret);
        return 0;
}

int sys_msg_send(int sockfd, const struct msghdr *msg, int flags, ssize_t *length) {
        auto ret = do_cp_syscall(SYS_sendmsg, sockfd, msg, flags);
        if (int e = sc_error(ret); e)
                return e;
        *length = sc_int_result<ssize_t>(ret);
        return 0;
}

int sys_msg_recv(int sockfd, struct msghdr *msg, int flags, ssize_t *length) {
        auto ret = do_cp_syscall(SYS_recvmsg, sockfd, msg, flags);
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
        auto ret = do_cp_syscall(SYS_fcntl, fd, cmd, arg);
        if (int e = sc_error(ret); e)
                return e;
        *result = sc_int_result<int>(ret);
        return 0;
}

int sys_getcwd(char *buf, size_t size) {
	auto ret = do_syscall(SYS_getcwd, buf, size);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int sys_unlinkat(int dfd, const char *path, int flags) {
	auto ret = do_syscall(SYS_unlinkat, dfd, path, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_sleep(time_t *secs, long *nanos) {
	struct timespec req = {
		.tv_sec = *secs,
		.tv_nsec = *nanos
	};
	struct timespec rem = {};

	auto ret = do_cp_syscall(SYS_nanosleep, &req, &rem);
        if (int e = sc_error(ret); e)
                return e;

	*secs = rem.tv_sec;
	*nanos = rem.tv_nsec;
	return 0;
}

int sys_isatty(int fd) {
	unsigned short winsizeHack[4];
	auto ret = do_syscall(SYS_ioctl, fd, 0x5413 /* TIOCGWINSZ */, &winsizeHack);
	if (int e = sc_error(ret); e)
		return e;
	auto res = sc_int_result<unsigned long>(ret);
	if(!res) return 0;
	return 1;
}

#ifdef __MLIBC_POSIX_OPTION

#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sched.h>

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
	auto ret = do_syscall(SYS_ioctl, fd, request, arg);
	if (int e = sc_error(ret); e)
		return e;
	if (result)
		*result = sc_int_result<unsigned long>(ret);
	return 0;
}

int sys_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
        auto ret = do_cp_syscall(SYS_connect, sockfd, addr, addrlen);
        if (int e = sc_error(ret); e)
                return e;
        return 0;
}

int sys_pselect(int nfds, fd_set *readfds, fd_set *writefds,
                fd_set *exceptfds, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
        // The Linux kernel really wants 7 arguments, even tho this is not supported
        // To fix that issue, they use a struct as the last argument.
        // See the man page of pselect and the glibc source code
        struct {
                sigset_t ss;
                size_t ss_len;
        } data;
        data.ss = (uintptr_t)sigmask;
        data.ss_len = NSIG / 8;

        auto ret = do_cp_syscall(SYS_pselect6, nfds, readfds, writefds,
                        exceptfds, timeout, &data);
        if (int e = sc_error(ret); e)
                return e;
        *num_events = sc_int_result<int>(ret);
        return 0;
}

int sys_pipe(int *fds, int flags) {
        if(flags) {
                auto ret = do_syscall(SYS_pipe2, fds, flags);
                if (int e = sc_error(ret); e)
                        return e;
                return 0;
        } else {
				auto ret = do_syscall(SYS_pipe2, fds, 0);
                if (int e = sc_error(ret); e)
                        return e;
                return 0;
        }
}

int sys_fork(pid_t *child) {
	auto ret = do_syscall(SYS_clone, SIGCHLD, 0);
	if (int e = sc_error(ret); e)
			return e;
	*child = sc_int_result<int>(ret);
	return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid) {
        auto ret = do_syscall(SYS_wait4, pid, status, flags, 0);
        if (int e = sc_error(ret); e)
                return e;
        *ret_pid = sc_int_result<int>(ret);
        return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
        auto ret = do_syscall(SYS_execve, path, argv, envp);
        if (int e = sc_error(ret); e)
                return e;
        return 0;
}

int sys_sigprocmask(int how, const sigset_t *set, sigset_t *old) {
        auto ret = do_syscall(SYS_rt_sigprocmask, how, set, old, NSIG / 8);
        if (int e = sc_error(ret); e)
                return e;
	return 0;
}

int sys_clone(void *entry, void *user_arg, void *tcb, pid_t *pid_out) {
        void *stack = prepare_stack(entry, user_arg);

	unsigned long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND
		| CLONE_THREAD | CLONE_SYSVSEM | CLONE_SETTLS | CLONE_SETTLS
		| CLONE_PARENT_SETTID;

#if defined(__riscv)
	// TP should point to the address immediately after the TCB.
	// TODO: We should change the sysdep so that we don't need to do this.
	auto tls = reinterpret_cast<char *>(tcb) + sizeof(Tcb);
	tcb = reinterpret_cast<void *>(tls);
#endif

	auto ret = __mlibc_spawn_thread(flags, stack, pid_out, NULL, tcb);
	if (ret < 0)
		return ret;

        return 0;
}

extern "C" const char __mlibc_syscall_begin[1];
extern "C" const char __mlibc_syscall_end[1];

#if defined(__riscv)
// Disable UBSan here to work around qemu-user misaligning ucontext_t.
// https://github.com/qemu/qemu/blob/2bf40d0841b942e7ba12953d515e62a436f0af84/linux-user/riscv/signal.c#L68-L69
[[gnu::no_sanitize("undefined")]]
#endif
int sys_before_cancellable_syscall(ucontext_t *uct) {
#if defined(__x86_64__)
	auto pc = reinterpret_cast<void*>(uct->uc_mcontext.gregs[REG_RIP]);
#elif defined(__riscv)
	auto pc = reinterpret_cast<void*>(uct->uc_mcontext.sc_regs.pc);
#else
#error "Missing architecture specific code."
#endif
	if (pc < __mlibc_syscall_begin || pc > __mlibc_syscall_end)
		return 0;
	return 1;
}

int sys_tgkill(int tgid, int tid, int sig) {
	auto ret = do_syscall(SYS_tgkill, tgid, tid, sig);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_tcgetattr(int fd, struct termios *attr) {
	auto ret = do_syscall(SYS_ioctl, fd, TCGETS, attr);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
	int req;

	switch (optional_action) {
		case TCSANOW: req = TCSETS; break;
		case TCSADRAIN: req = TCSETSW; break;
		case TCSAFLUSH: req = TCSETSF; break;
		default: return EINVAL;
	}

	auto ret = do_syscall(SYS_ioctl, fd, req, attr);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_access(const char *path, int mode) {
	auto ret = do_syscall(SYS_faccessat, AT_FDCWD, path, mode, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length) {
	auto ret = do_syscall(SYS_accept, fd, addr_ptr, addr_length, 0, 0, 0);
	if (int e = sc_error(ret); e)
		return e;
	*newfd = fd;
	return 0;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	auto ret = do_syscall(SYS_bind, fd, addr_ptr, addr_length, 0, 0, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
	auto ret = do_syscall(SYS_setsockopt, fd, layer, number, buffer, size, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_listen(int fd, int backlog) {
	auto ret = do_syscall(SYS_listen, fd, backlog, 0, 0, 0, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_getpriority(int which, id_t who, int *value) {
	auto ret = do_syscall(SYS_getpriority, which, who);
	if (int e = sc_error(ret); e) {
		return e;
	}
	*value = 20 - sc_int_result<int>(ret);
	return 0;
}

int sys_setpriority(int which, id_t who, int prio) {
	auto ret = do_syscall(SYS_setpriority, which, who, prio);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
	auto ret = do_syscall(SYS_setitimer, which, new_value, old_value);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_ptrace(long req, pid_t pid, void *addr, void *data, long *out) {
	auto ret = do_syscall(SYS_ptrace, req, pid, addr, data);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<long>(ret);
	return 0;
}

int sys_open_dir(const char *path, int *fd) {
	return sys_open(path, O_DIRECTORY, 0, fd);
}

int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	auto ret = do_syscall(NR_getdents64, handle, buffer, max_size);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<int>(ret);
	return 0;
}

#endif // __MLIBC_POSIX_OPTION

pid_t sys_getpid() {
	auto ret = do_syscall(SYS_getpid);
	// getpid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

uid_t sys_getuid() {
	auto ret = do_syscall(SYS_getuid);
	// getuid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

uid_t sys_geteuid() {
	auto ret = do_syscall(SYS_geteuid);
	// geteuid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

gid_t sys_getgid() {
	auto ret = do_syscall(SYS_getgid);
	// getgid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

gid_t sys_getegid() {
	auto ret = do_syscall(SYS_getegid);
	// getegid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

int sys_kill(int pid, int sig) {
	auto ret = do_syscall(SYS_kill, pid, sig);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
	auto ret = do_syscall(SYS_mprotect, pointer, size, prot);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

void sys_thread_exit() {
	do_syscall(SYS_exit, 0);
	__builtin_trap();
}

void sys_exit(int status) {
	do_syscall(SYS_exit_group, status);
	__builtin_trap();
}

#endif // MLIBC_BUILDING_RTDL

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	auto ret = do_cp_syscall(SYS_sys_futex, pointer, FUTEX_WAIT, expected, time);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_futex_wake(int *pointer) {
	auto ret = do_syscall(SYS_sys_futex, pointer, FUTEX_WAKE, INT_MAX);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_sigsuspend(const sigset_t *set) {
	auto ret = do_syscall(SYS_rt_sigsuspend, set, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_sigaltstack(const stack_t *ss, stack_t *oss) {
	auto ret = do_syscall(SYS_sigaltstack, ss, oss);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_mkdir(const char *path, mode_t mode) {
	auto ret = do_syscall(SYS_mkdirat, AT_FDCWD, path, mode);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}


int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	auto ret = do_syscall(SYS_mkdirat, dirfd, path, mode);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_symlink(const char *target_path, const char *link_path) {
	auto ret = do_syscall(SYS_symlinkat, target_path, AT_FDCWD, link_path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_chdir(const char *path) {
	auto ret = do_syscall(SYS_chdir, path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_rmdir(const char *path) {
	auto ret = do_syscall(SYS_unlinkat, AT_FDCWD, path, AT_REMOVEDIR);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_readlink(const char *path, void *buf, size_t bufsiz, ssize_t *len) {
	auto ret = do_syscall(SYS_readlinkat, AT_FDCWD, path, buf, bufsiz);
	if (int e = sc_error(ret); e)
		return e;
	*len = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_getrlimit(int resource, struct rlimit *limit) {
	auto ret = do_syscall(SYS_getrlimit, resource, limit);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_setrlimit(int resource, const struct rlimit *limit) {
	auto ret = do_syscall(SYS_setrlimit, resource, limit);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

pid_t sys_getppid() {
	auto ret = do_syscall(NR_getppid);
	// getppid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	auto ret = do_syscall(NR_setpgid, pid, pgid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_getpgid(pid_t pid, pid_t *out) {
	auto ret = do_syscall(NR_getpgid, pid);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<pid_t>(ret);
	return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
	auto ret = do_cp_syscall(NR_dup, fd);
	if (int e = sc_error(ret); e)
		return e;
	// TODO: Handle flags
	(void) flags;
	*newfd = sc_int_result<int>(ret);
	return 0;
}

} // namespace mlibc
