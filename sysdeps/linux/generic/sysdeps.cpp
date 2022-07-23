#include <errno.h>
#include <limits.h>
#include <linux/reboot.h>

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
#elif defined (__aarch64__)
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb) - 0x10;
	asm volatile ("msr tpidr_el0, %0" :: "r"(thread_data));
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

int sys_fadvise(int fd, off_t offset, off_t length, int advice) {
	auto ret = do_syscall(SYS_fadvise64, fd, offset, length, advice);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	auto ret = do_cp_syscall(SYS_openat, AT_FDCWD, path, flags, mode);
	if(int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	auto ret = do_syscall(SYS_openat, dirfd, path, flags, mode);
	if (int e = sc_error(ret); e)
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

int sys_chmod(const char *pathname, mode_t mode) {
	auto ret = do_cp_syscall(SYS_fchmodat, AT_FDCWD, pathname, mode);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_fchmod(int fd, mode_t mode) {
	auto ret = do_cp_syscall(SYS_fchmod, fd, mode);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
	auto ret = do_cp_syscall(SYS_fchmodat, fd, pathname, mode, flags);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
	auto ret = do_cp_syscall(SYS_fchownat, dirfd, pathname, owner, group, flags);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	auto ret = do_cp_syscall(SYS_utimensat, dirfd, pathname, times, flags);
	if (int e = sc_error(ret); e)
		return e;
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
	auto ret = do_syscall(SYS_munmap, pointer, size);
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

int sys_clock_getres(int clock, time_t *secs, long *nanos) {
	struct timespec tp = {};
	auto ret = do_syscall(SYS_clock_getres, clock, &tp);
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

int sys_statfs(const char *path, struct statfs *buf) {
	auto ret = do_cp_syscall(SYS_statfs, path, buf);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_fstatfs(int fd, struct statfs *buf) {
	auto ret = do_cp_syscall(SYS_fstatfs, fd, buf);
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

#if __MLIBC_POSIX_OPTION

#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <fcntl.h>

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

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	auto ret = do_syscall(SYS_wait4, pid, status, flags, ru);
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

int sys_setresuid(uid_t ruid, uid_t euid, uid_t suid) {
	auto ret = do_syscall(SYS_setresuid, ruid, euid, suid);
        if (int e = sc_error(ret); e)
                return e;
	return 0;
}

int sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid) {
	auto ret = do_syscall(SYS_setresgid, rgid, egid, sgid);
        if (int e = sc_error(ret); e)
                return e;
	return 0;
}

int sys_setreuid(uid_t ruid, uid_t euid) {
	auto ret = do_syscall(SYS_setreuid, ruid, euid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_setregid(gid_t rgid, gid_t egid) {
	auto ret = do_syscall(SYS_setregid, rgid, egid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_sysinfo(struct sysinfo *info) {
	auto ret = do_syscall(SYS_sysinfo, info);
        if (int e = sc_error(ret); e)
                return e;
	return 0;
}

void sys_yield() {
	do_syscall(SYS_sched_yield);
}

int sys_clone(void *tcb, pid_t *pid_out, void *stack) {
	unsigned long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND
		| CLONE_THREAD | CLONE_SYSVSEM | CLONE_SETTLS | CLONE_SETTLS
		| CLONE_PARENT_SETTID;

#if defined(__riscv)
	// TP should point to the address immediately after the TCB.
	// TODO: We should change the sysdep so that we don't need to do this.
	auto tls = reinterpret_cast<char *>(tcb) + sizeof(Tcb);
	tcb = reinterpret_cast<void *>(tls);
#elif defined(__aarch64__)
	// TP should point to the address 16 bytes before the end of the TCB.
	// TODO: We should change the sysdep so that we don't need to do this.
	auto tp = reinterpret_cast<char *>(tcb) + sizeof(Tcb) - 0x10;
	tcb = reinterpret_cast<void *>(tp);
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
#elif defined(__aarch64__)
	auto pc = reinterpret_cast<void*>(uct->uc_mcontext.pc);
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

int sys_tcdrain(int fd) {
	auto ret = do_syscall(SYS_ioctl, fd, TCSBRK, 1);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_tcflow(int fd, int action) {
	auto ret = do_syscall(SYS_ioctl, fd, TCXONC, action);
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

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
	auto ret = do_syscall(SYS_faccessat, dirfd, pathname, mode, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length) {
	auto ret = do_syscall(SYS_accept, fd, addr_ptr, addr_length, 0, 0, 0);
	if (int e = sc_error(ret); e)
		return e;
	*newfd = sc_int_result<int>(ret);
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

int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	auto ret = do_syscall(SYS_getsockname, fd, addr_ptr, &max_addr_length);
	if (int e = sc_error(ret); e)
		return e;
	*actual_length = max_addr_length;
	return 0;
}

int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	auto ret = do_syscall(SYS_getpeername, fd, addr_ptr, &max_addr_length);
	if (int e = sc_error(ret); e)
		return e;
	*actual_length = max_addr_length;
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
	auto ret = do_syscall(SYS_getdents64, handle, buffer, max_size);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<int>(ret);
	return 0;
}

int sys_prctl(int op, va_list ap, int *out) {
	unsigned long x[4];
	for(int i = 0; i < 4; i++)
		x[i] = va_arg(ap, unsigned long);

	auto ret = do_syscall(SYS_prctl, op, x[0], x[1], x[2], x[3]);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);
	return 0;
}

int sys_uname(struct utsname *buf) {
	auto ret = do_syscall(SYS_uname, buf);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_gethostname(char *buf, size_t bufsize) {
	struct utsname uname_buf;
	if (auto e = sys_uname(&uname_buf); e)
		return e;

	auto node_len = strlen(uname_buf.nodename);
	if (node_len >= bufsize)
		return ENAMETOOLONG;

	memcpy(buf, uname_buf.nodename, node_len);
	buf[node_len] = '\0';
	return 0;
}

int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	auto ret = do_syscall(SYS_pread64, fd, buf, n, off);
	if (int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
	auto ret = do_syscall(SYS_pwrite64, fd, buf, n, off);
	if (int e = sc_error(ret); e)
		return e;
	*bytes_written = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	struct timespec tm;
	tm.tv_sec = timeout / 1000;
	tm.tv_nsec = timeout % 1000 * 1000000;
	auto ret = do_syscall(SYS_ppoll, fds, count, timeout >= 0 ? &tm : nullptr, 0, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	*num_events = sc_int_result<int>(ret);
	return 0;
}

int sys_getrusage(int scope, struct rusage *usage) {
	auto ret = do_syscall(SYS_getrusage, scope, usage);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_madvise(void *addr, size_t length, int advice) {
	auto ret = do_syscall(SYS_madvise, addr, length, advice);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_msync(void *addr, size_t length, int flags) {
	auto ret = do_syscall(SYS_msync, addr, length, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_reboot(int cmd) {
	auto ret = do_syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, cmd, nullptr);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask) {
	auto ret = do_syscall(SYS_sched_getaffinity, pid, cpusetsize, mask);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_mount(const char *source, const char *target,
	const char *fstype, unsigned long flags, const void *data) {
	auto ret = do_syscall(SYS_mount, source, target, fstype, flags, data);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_umount2(const char *target, int flags) {
	auto ret = do_syscall(SYS_umount2, target, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_sethostname(const char *buffer, size_t bufsize) {
	auto ret = do_syscall(SYS_sethostname, buffer, bufsize);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_epoll_create(int flags, int *fd) {
	auto ret = do_syscall(SYS_epoll_create1, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev) {
	auto ret = do_syscall(SYS_epoll_ctl, epfd, mode, fd, ev);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_epoll_pwait(int epfd, struct epoll_event *ev, int n, int timeout, const sigset_t *sigmask, int *raised) {
	auto ret = do_syscall(SYS_epoll_pwait, epfd, ev, n, timeout, sigmask, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	*raised = sc_int_result<int>(ret);
	return 0;
}

int sys_eventfd_create(unsigned int initval, int flags, int *fd) {
	auto ret = do_syscall(SYS_eventfd2, initval, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_signalfd_create(const sigset_t *masks, int flags, int *fd) {
	auto ret = do_syscall(SYS_signalfd4, *fd, masks, sizeof(sigset_t), flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_timerfd_create(int clockid, int flags, int *fd) {
	auto ret = do_syscall(SYS_timerfd_create, clockid, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_timerfd_settime(int fd, int flags, const struct itimerspec *value, struct itimerspec *oldvalue) {
	auto ret = do_syscall(SYS_timerfd_settime, fd, flags, value, oldvalue);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_inotify_create(int flags, int *fd) {
	auto ret = do_syscall(SYS_inotify_init1, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_init_module(void *module, unsigned long length, const char *args) {
	auto ret = do_syscall(SYS_init_module, module, length, args);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_delete_module(const char *name, unsigned flags) {
	auto ret = do_syscall(SYS_delete_module, name, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_klogctl(int type, char *bufp, int len, int *out) {
	auto ret = do_syscall(SYS_syslog, type, bufp, len);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);
	return 0;
}

int sys_getcpu(int *cpu) {
	auto ret = do_syscall(SYS_getcpu, cpu, NULL, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) {
	auto ret = do_syscall(SYS_socketpair, domain, type_and_flags, proto, fds, 0, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
	auto ret = do_syscall(SYS_getsockopt, fd, layer, number, buffer, size, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_inotify_add_watch(int ifd, const char *path, uint32_t mask, int *wd) {
	auto ret = do_syscall(SYS_inotify_add_watch, ifd, path, mask);
	if (int e = sc_error(ret); e)
		return e;
	*wd = sc_int_result<int>(ret);
	return 0;
}

int sys_inotify_rm_watch(int ifd, int wd) {
	auto ret = do_syscall(SYS_inotify_rm_watch, ifd, wd);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_ttyname(int fd, char *buf, size_t size) {
	if (!isatty(fd))
		return errno;

	char *procname;
	if(int e = asprintf(&procname, "/proc/self/fd/%i", fd); e)
		return ENOMEM;
	__ensure(procname);

	ssize_t l = readlink(procname, buf, size);
	free(procname);

	if (l < 0)
		return errno;
	else if ((size_t)l >= size)
		return ERANGE;

	buf[l] = '\0';
	struct stat st1;
	struct stat st2;

	if (stat(buf, &st1) || fstat(fd, &st2))
		return errno;
	if (st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino)
		return ENODEV;

	return 0;
}

int sys_pause() {
#ifdef SYS_pause
	auto ret = do_syscall(SYS_pause);
#else
	auto ret = do_syscall(SYS_ppoll, 0, 0, 0, 0);
#endif
	if (int e = sc_error(ret); e)
		return e;
	return EINTR;
}

int sys_mlockall(int flags) {
	auto ret = do_syscall(SYS_mlockall, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#endif // __MLIBC_POSIX_OPTION

int sys_times(struct tms *tms, clock_t *out) {
	auto ret = do_syscall(SYS_times, tms);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<long>(ret);
	return 0;
}

pid_t sys_getpid() {
	auto ret = do_syscall(SYS_getpid);
	// getpid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

pid_t sys_gettid() {
	auto ret = do_syscall(SYS_gettid);
	// gettid() always succeeds.
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

int sys_futex_tid() {
	auto ret = do_syscall(SYS_gettid);
	// gettid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	auto ret = do_cp_syscall(SYS_futex, pointer, FUTEX_WAIT, expected, time);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_futex_wake(int *pointer) {
	auto ret = do_syscall(SYS_futex, pointer, FUTEX_WAKE, INT_MAX);
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

int sys_mknodat(int dirfd, const char *path, int mode, int dev) {
	auto ret = do_syscall(SYS_mknodat, dirfd, path, mode, dev);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_mkfifoat(int dirfd, const char *path, int mode) {
	return sys_mknodat(dirfd, path, mode | S_IFIFO, 0);
}

int sys_symlink(const char *target_path, const char *link_path) {
	auto ret = do_syscall(SYS_symlinkat, target_path, AT_FDCWD, link_path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
	auto ret = do_syscall(SYS_symlinkat, target_path, dirfd, link_path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_umask(mode_t mode, mode_t *old) {
	auto ret = do_syscall(SYS_umask, mode);
	if (int e = sc_error(ret); e)
		return e;
	*old = sc_int_result<mode_t>(ret);
	return 0;
}

int sys_chdir(const char *path) {
	auto ret = do_syscall(SYS_chdir, path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_fchdir(int fd) {
	auto ret = do_syscall(SYS_fchdir, fd);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_rename(const char *old_path, const char *new_path) {
	return sys_renameat(AT_FDCWD, old_path, AT_FDCWD, new_path);
}

int sys_renameat(int old_dirfd, const char *old_path, int new_dirfd, const char *new_path) {
#ifdef SYS_renameat2
	auto ret = do_syscall(SYS_renameat2, old_dirfd, old_path, new_dirfd, new_path, 0);
#else
	auto ret = do_syscall(SYS_renameat, old_dirfd, old_path, new_dirfd, new_path);
#endif /* defined(SYS_renameat2) */
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

int sys_ftruncate(int fd, size_t size) {
	auto ret = do_syscall(SYS_ftruncate, fd, size);
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
	auto ret = do_syscall(SYS_getppid);
	// getppid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	auto ret = do_syscall(SYS_setpgid, pid, pgid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

pid_t sys_getsid(pid_t pid, pid_t *sid) {
	auto ret = do_syscall(SYS_getsid, pid);
	if (int e = sc_error(ret); e)
		return e;
	*sid = sc_int_result<pid_t>(ret);
	return 0;
}

int sys_setsid(pid_t *sid) {
	auto ret = do_syscall(SYS_setsid);
	if (int e = sc_error(ret); e)
		return e;
	*sid = sc_int_result<pid_t>(ret);
	return 0;
}

int sys_setuid(uid_t uid) {
	auto ret = do_syscall(SYS_setuid, uid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_setgid(gid_t gid) {
	auto ret = do_syscall(SYS_setgid, gid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_getpgid(pid_t pid, pid_t *out) {
	auto ret = do_syscall(SYS_getpgid, pid);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<pid_t>(ret);
	return 0;
}

int sys_getgroups(size_t size, const gid_t *list, int *retval) {
	auto ret = do_syscall(SYS_getgroups, size, list);
	if (int e = sc_error(ret); e)
		return e;
	*retval = sc_int_result<int>(ret);
	return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
	__ensure(!flags);
	auto ret = do_cp_syscall(SYS_dup, fd);
	if (int e = sc_error(ret); e)
		return e;
	*newfd = sc_int_result<int>(ret);
	return 0;
}

void sys_sync() {
	do_syscall(SYS_sync);
}

int sys_fsync(int fd) {
	auto ret = do_syscall(SYS_fsync, fd);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_fdatasync(int fd) {
	auto ret = do_syscall(SYS_fdatasync, fd);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_getrandom(void *buffer, size_t length, int flags, ssize_t *bytes_written) {
	auto ret = do_syscall(SYS_getrandom, buffer, length, flags);
	if (int e = sc_error(ret); e)
		return e;
	*bytes_written = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_getentropy(void *buffer, size_t length) {
	ssize_t written;
	return sys_getrandom(buffer, length, 0, &written);
}

} // namespace mlibc
