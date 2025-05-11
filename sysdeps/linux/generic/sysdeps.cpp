#include <asm/ioctls.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include <type_traits>

#include <mlibc-config.h>
#include <bits/ensure.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/socklen_t.h>
#if __MLIBC_LINUX_OPTION
#  include <abi-bits/statx.h>
#endif
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <limits.h>
#include <sys/syscall.h>
#include "cxx-syscall.hpp"

#if __MLIBC_LINUX_OPTION && !defined(MLIBC_BUILDING_RTLD)

#include "generic-helpers/netlink.hpp"

#endif

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }
#define UNUSED(x) (void)(x);

#ifndef MLIBC_BUILDING_RTLD
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

#if defined(__i386__)

struct user_desc {
	unsigned int entry_number;
	unsigned long base_addr;
	unsigned int limit;
	unsigned int seg_32bit: 1;
	unsigned int contents: 2;
	unsigned int read_exec_only: 1;
	unsigned int limit_in_pages: 1;
	unsigned int seg_not_present: 1;
	unsigned int useable: 1;
};

#endif

#if defined(__m68k__)
extern "C" void *__m68k_read_tp() {
	auto ret = do_syscall(__NR_get_thread_area, 0);
	return (void *)ret;
}
#endif

int sys_tcb_set(void *pointer) {
#if defined(__x86_64__)
	auto ret = do_syscall(SYS_arch_prctl, 0x1002 /* ARCH_SET_FS */, pointer);
	if(int e = sc_error(ret); e)
		return e;
#elif defined(__i386__)
	struct user_desc desc = {
		.entry_number = static_cast<unsigned int>(-1),
		.base_addr = uintptr_t(pointer),
		.limit = 0xfffff,
		.seg_32bit = 1,
		.contents = 0,
		.read_exec_only = 0,
		.limit_in_pages = 1,
		.seg_not_present = 0,
		.useable = 1,
	};
	auto ret = do_syscall(SYS_set_thread_area, &desc);
	__ensure(!sc_error(ret));
	asm volatile ("movw %w0, %%gs" : : "q"(desc.entry_number * 8 + 3) :);
#elif defined(__riscv)
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile ("mv tp, %0" :: "r"(thread_data));
#elif defined (__aarch64__)
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb) - 0x10;
	asm volatile ("msr tpidr_el0, %0" :: "r"(thread_data));
#elif defined (__m68k__)
	auto ret = do_syscall(__NR_set_thread_area, (uintptr_t)pointer + 0x7000 + sizeof(Tcb));
	if(int e = sc_error(ret); e)
		return e;
#elif defined(__loongarch64)
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile ("move $tp, %0" :: "r"(thread_data));
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

int sys_readv(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read) {
	auto ret = do_cp_syscall(SYS_readv, fd, iovs, iovc);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_write(int fd, const void *buffer, size_t size, ssize_t *bytes_written) {
	auto ret = do_cp_syscall(SYS_write, fd, buffer, size);
	if(int e = sc_error(ret); e)
		return e;
	if(bytes_written)
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
	if(offset % 4096)
		return EINVAL;
	if(size >= PTRDIFF_MAX)
		return ENOMEM;
#if defined(SYS_mmap2)
	auto ret = do_syscall(SYS_mmap2, hint, size, prot, flags, fd, offset/4096);
#else
	auto ret = do_syscall(SYS_mmap, hint, size, prot, flags, fd, offset);
#endif
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

int sys_vm_protect(void *pointer, size_t size, int prot) {
	auto ret = do_syscall(SYS_mprotect, pointer, size, prot);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

// All remaining functions are disabled in ldso.
#ifndef MLIBC_BUILDING_RTLD

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

#if defined(SYS_newfstatat)
	auto ret = do_cp_syscall(SYS_newfstatat, fd, path, statbuf, flags);
#else
	auto ret = do_cp_syscall(SYS_fstatat64, fd, path, statbuf, flags);
#endif
	if (int e = sc_error(ret); e) {
		return e;
	}

#if defined(__i386__)
	statbuf->st_atim.tv_sec = statbuf->__st_atim32.tv_sec;
	statbuf->st_atim.tv_nsec = statbuf->__st_atim32.tv_nsec;
	statbuf->st_mtim.tv_sec = statbuf->__st_mtim32.tv_sec;
	statbuf->st_mtim.tv_nsec = statbuf->__st_mtim32.tv_nsec;
	statbuf->st_ctim.tv_sec = statbuf->__st_ctim32.tv_sec;
	statbuf->st_ctim.tv_nsec = statbuf->__st_ctim32.tv_nsec;
#endif

	return 0;
}

#if __MLIBC_LINUX_OPTION
static_assert(sizeof(struct statx) == 0x100); // Linux kernel requires it to be precisely 256 bytes.

int sys_statx(int dirfd, const char *path, int flags, unsigned int mask, struct statx *statxbuf) {
	auto ret = do_cp_syscall(SYS_statx, dirfd, path, flags, mask, statxbuf);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}
#endif

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
extern "C" void __mlibc_signal_restore_rt(void);

int sys_sigaction(int signum, const struct sigaction *act,
		struct sigaction *oldact) {
	struct ksigaction {
		void (*handler)(int);
		unsigned long flags;
		void (*restorer)(void);
		uint32_t mask[2];
	};

	struct ksigaction kernel_act, kernel_oldact;
	if (act) {
		kernel_act.handler = act->sa_handler;
		kernel_act.flags = act->sa_flags | SA_RESTORER;
		kernel_act.restorer = (act->sa_flags & SA_SIGINFO) ? __mlibc_signal_restore_rt : __mlibc_signal_restore;
		memcpy(&kernel_act.mask, &act->sa_mask, sizeof(kernel_act.mask));
	}

	static_assert(sizeof(kernel_act.mask) == 8);

	auto ret = do_syscall(SYS_rt_sigaction, signum, act ?
		&kernel_act : NULL, oldact ?
		&kernel_oldact : NULL, sizeof(kernel_act.mask));
	if (int e = sc_error(ret); e)
		return e;

	if (oldact) {
		oldact->sa_handler = kernel_oldact.handler;
		oldact->sa_flags = kernel_oldact.flags;
		oldact->sa_restorer = kernel_oldact.restorer;
		memcpy(&oldact->sa_mask, &kernel_oldact.mask, sizeof(kernel_oldact.mask));
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

ssize_t sys_sendto(int fd, const void *buffer, size_t size, int flags, const struct sockaddr *sock_addr, socklen_t addr_length, ssize_t *length) {
	auto ret = do_cp_syscall(SYS_sendto, fd, buffer, size, flags, sock_addr, addr_length);
	if(int e = sc_error(ret); e) {
		return e;
	}
	*length = sc_int_result<ssize_t>(ret);
	return 0;
}

ssize_t sys_recvfrom(int fd, void *buffer, size_t size, int flags, struct sockaddr *sock_addr, socklen_t *addr_length, ssize_t *length) {
	auto ret = do_cp_syscall(SYS_recvfrom, fd, buffer, size, flags, sock_addr, addr_length);
	if(int e = sc_error(ret); e) {
		return e;
	}
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

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/user.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <fcntl.h>
#include <pthread.h>

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
	// The Linux kernel sometimes modifies the timeout argument.
	struct timespec local_timeout;
	if(timeout)
		local_timeout = *timeout;

	// The Linux kernel really wants 7 arguments, even tho this is not supported
	// To fix that issue, they use a struct as the last argument.
	// See the man page of pselect and the glibc source code
	struct {
		const sigset_t *sigmask;
		size_t ss_len;
	} data;
	data.sigmask = sigmask;
	data.ss_len = NSIG / 8;

	auto ret = do_cp_syscall(SYS_pselect6, nfds, readfds, writefds,
			exceptfds, timeout ? &local_timeout : nullptr, &data);
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
	*ret_pid = sc_int_result<pid_t>(ret);
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

int sys_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) {
	auto ret = do_syscall(SYS_getresuid, ruid, euid, suid);
        if (int e = sc_error(ret); e)
                return e;
	return 0;
}

int sys_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	auto ret = do_syscall(SYS_getresgid, rgid, egid, sgid);
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
		| CLONE_THREAD | CLONE_SYSVSEM | CLONE_SETTLS
		| CLONE_PARENT_SETTID;

#if defined(__riscv) || defined(__loongarch64)
	// TP should point to the address immediately after the TCB.
	// TODO: We should change the sysdep so that we don't need to do this.
	auto tls = reinterpret_cast<char *>(tcb) + sizeof(Tcb);
	tcb = reinterpret_cast<void *>(tls);
#elif defined(__aarch64__)
	// TP should point to the address 16 bytes before the end of the TCB.
	// TODO: We should change the sysdep so that we don't need to do this.
	auto tp = reinterpret_cast<char *>(tcb) + sizeof(Tcb) - 0x10;
	tcb = reinterpret_cast<void *>(tp);
#elif defined(__i386__)
	/* get the entry number, as we don't request a new one here */
	uint32_t gs;
	asm volatile("movw %%gs, %w0" : "=q"(gs));

	auto user_desc = reinterpret_cast<struct user_desc *>(getAllocator().allocate(sizeof(struct user_desc)));

	user_desc->entry_number = (gs & 0xffff) >> 3;
	user_desc->base_addr = uintptr_t(tcb);
	user_desc->limit = 0xfffff;
	user_desc->seg_32bit = 1;
	user_desc->contents = 0;
	user_desc->read_exec_only = 0;
	user_desc->limit_in_pages = 1;
	user_desc->seg_not_present = 0;
	user_desc->useable = 1;

	tcb = reinterpret_cast<void *>(user_desc);
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
#elif defined(__i386__)
	auto pc = reinterpret_cast<void*>(uct->uc_mcontext.gregs[REG_EIP]);
#elif defined(__riscv)
	auto pc = reinterpret_cast<void*>(uct->uc_mcontext.gregs[REG_PC]);
#elif defined(__aarch64__)
	auto pc = reinterpret_cast<void*>(uct->uc_mcontext.pc);
#elif defined(__m68k__)
	auto pc = reinterpret_cast<void*>(uct->uc_mcontext.gregs[R_PC]);
#elif defined(__loongarch64)
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

int sys_tcflush(int fd, int queue) {
	auto ret = do_syscall(SYS_ioctl, fd, TCFLSH, queue);
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

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	auto ret = do_syscall(SYS_accept4, fd, addr_ptr, addr_length, flags);
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

int sys_shutdown(int sockfd, int how) {
	auto ret = do_syscall(SYS_shutdown, sockfd, how);
	if (int e = sc_error(ret); e) {
		return e;
	}
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

/* Linux' uapi does some weird union stuff in place of `sigev_tid`, which we conveniently ignore */
struct linux_uapi_sigevent {
	union sigval sigev_value;
	int sigev_signo;
	int sigev_notify;
	int sigev_tid;
};

int sys_timer_create(clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res) {
	struct linux_uapi_sigevent ksev;
	struct linux_uapi_sigevent *ksevp = 0;
	int timer_id;

	switch(evp ? evp->sigev_notify : SIGEV_SIGNAL) {
		case SIGEV_NONE:
		case SIGEV_SIGNAL: {
			if(evp) {
				ksev.sigev_value = evp->sigev_value;
				ksev.sigev_signo = evp->sigev_signo;
				ksev.sigev_notify = evp->sigev_notify;
				ksev.sigev_tid = 0;
				ksevp = &ksev;
			}

			auto ret = do_syscall(SYS_timer_create, clk, ksevp, &timer_id);
			if (int e = sc_error(ret); e) {
				return e;
			}
			*res = (void *) (intptr_t) timer_id;
			break;
		}
		case SIGEV_THREAD:
			__ensure(!"sys_timer_create with evp->sigev_notify == SIGEV_THREAD is unimplemented");
			[[fallthrough]];
		default:
			return EINVAL;
	}

	return 0;
}

int sys_timer_settime(timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old) {
	auto ret = do_syscall(SYS_timer_settime, t, flags, val, old);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int sys_timer_delete(timer_t t) {
	__ensure((intptr_t) t >= 0);
	auto ret = do_syscall(SYS_timer_delete, t);
	if (int e = sc_error(ret); e) {
		return e;
	}
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

int sys_swapon(const char *path, int flags) {
	auto ret = do_syscall(SYS_swapon, path, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_swapoff(const char *path) {
	auto ret = do_syscall(SYS_swapoff, path);
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
	auto ret = do_syscall(SYS_signalfd4, *fd, masks, NSIG / 8, flags);
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

int sys_timerfd_gettime(int fd, struct itimerspec *its) {
	auto ret = do_syscall(SYS_timerfd_gettime, fd, its);
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

int sys_get_min_priority(int policy, int *out) {
	auto ret = do_syscall(SYS_sched_get_priority_min, policy);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);

	return 0;
}

int sys_getschedparam(void *tcb, int *policy, struct sched_param *param) {
	auto t = reinterpret_cast<Tcb *>(tcb);

	if(!t->tid) {
		return ESRCH;
	}

	auto ret_param = do_syscall(SYS_sched_getparam, t->tid, param);
	if (int e = sc_error(ret_param); e)
		return e;

	auto ret_sched = do_syscall(SYS_sched_getscheduler, t->tid, param);
	if (int e = sc_error(ret_sched); e)
		return e;
	*policy = sc_int_result<int>(ret_sched);

	return 0;
}

int sys_setschedparam(void *tcb, int policy, const struct sched_param *param) {
	auto t = reinterpret_cast<Tcb *>(tcb);

	if(!t->tid) {
		return ESRCH;
	}

	auto ret = do_syscall(SYS_sched_setscheduler, t->tid, policy, param);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_getparam(pid_t pid, struct sched_param *param) {
	auto ret = do_syscall(SYS_sched_getparam, pid, param);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_setparam(pid_t pid, const struct sched_param *param) {
	auto ret = do_syscall(SYS_sched_setparam, pid, param);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_if_indextoname(unsigned int index, char *name) {
	int fd = 0;
	int r = sys_socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if(r)
		return r;

	struct ifreq ifr;
	ifr.ifr_ifindex = index;

	int ret = sys_ioctl(fd, SIOCGIFNAME, &ifr, NULL);
	close(fd);

	if(ret) {
		if(ret == ENODEV)
			return ENXIO;
		return ret;
	}

	strncpy(name, ifr.ifr_name, IF_NAMESIZE);

	return 0;
}

int sys_if_nametoindex(const char *name, unsigned int *ret) {
	int fd = 0;
	int r = sys_socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if(r)
		return r;

	struct ifreq ifr;
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);

	r = sys_ioctl(fd, SIOCGIFINDEX, &ifr, NULL);
	close(fd);

	if(r) {
		return r;
	}

	*ret = ifr.ifr_ifindex;

	return 0;
}

int sys_ptsname(int fd, char *buffer, size_t length) {
	int index;
	if(int e = sys_ioctl(fd, TIOCGPTN, &index, NULL); e)
		return e;
	if((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
		return ERANGE;
	}
	return 0;
}

int sys_unlockpt(int fd) {
	int unlock = 0;

	if(int e = sys_ioctl(fd, TIOCSPTLCK, &unlock, NULL); e)
		return e;

	return 0;
}

int sys_thread_setname(void *tcb, const char *name) {
	if(strlen(name) > 15) {
		return ERANGE;
	}

	auto t = reinterpret_cast<Tcb *>(tcb);
	char *path;
	int cs = 0;

	if(asprintf(&path, "/proc/self/task/%d/comm", t->tid) < 0) {
		return ENOMEM;
	}

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	int fd;
	if(int e = sys_open(path, O_WRONLY, 0, &fd); e) {
		return e;
	}

	if(int e = sys_write(fd, name, strlen(name) + 1, NULL)) {
		return e;
	}

	sys_close(fd);

	pthread_setcancelstate(cs, 0);

	return 0;
}

int sys_thread_getname(void *tcb, char *name, size_t size) {
	auto t = reinterpret_cast<Tcb *>(tcb);
	char *path;
	int cs = 0;
	ssize_t real_size = 0;

	if(asprintf(&path, "/proc/self/task/%d/comm", t->tid) < 0) {
		return ENOMEM;
	}

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	int fd;
	if(int e = sys_open(path, O_RDONLY | O_CLOEXEC, 0, &fd); e) {
		return e;
	}

	if(int e = sys_read(fd, name, size, &real_size)) {
		return e;
	}

	name[real_size - 1] = 0;
	sys_close(fd);

	pthread_setcancelstate(cs, 0);

	if(static_cast<ssize_t>(size) <= real_size) {
		return ERANGE;
	}

	return 0;
}

int sys_mlock(const void *addr, size_t length) {
	auto ret = do_syscall(SYS_mlock, addr, length);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_munlock(const void *addr, size_t length) {
	auto ret = do_syscall(SYS_munlock, addr, length);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_munlockall(void) {
	auto ret = do_syscall(SYS_munlockall);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_mincore(void *addr, size_t length, unsigned char *vec) {
	auto ret = do_syscall(SYS_mincore, addr, length, vec);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_memfd_create(const char *name, int flags, int *fd) {
	auto ret = do_syscall(SYS_memfd_create, name, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_fallocate(int fd, off_t offset, size_t size) {
	auto ret = do_syscall(SYS_fallocate, fd, 0, offset, size);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_flock(int fd, int options) {
	auto ret = do_syscall(SYS_flock, fd, options);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_seteuid(uid_t euid) {
	return sys_setresuid(-1, euid, -1);
}

int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window) {
	auto ret = do_syscall(SYS_mremap, pointer, size, new_size, MREMAP_MAYMOVE);
	// TODO: musl fixes up EPERM errors from the kernel.
	if(int e = sc_error(ret); e)
		return e;
	*window = sc_ptr_result<void>(ret);
	return 0;
}

int sys_link(const char *old_path, const char *new_path) {
#ifdef SYS_link
	auto ret = do_syscall(SYS_link, old_path, new_path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
#else
	return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
#endif
}

int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	auto ret = do_syscall(SYS_linkat, olddirfd, old_path, newdirfd, new_path, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

// Inspired by musl (src/stat/statvfs.c:28 fixup function)
static void statfs_to_statvfs(struct statfs *from, struct statvfs *to) {
	*to = {
		.f_bsize = from->f_bsize,
		.f_frsize = from->f_frsize ? from->f_frsize : from->f_bsize,
		.f_blocks = from->f_blocks,
		.f_bfree = from->f_bfree,
		.f_bavail = from->f_bavail,
		.f_files = from->f_files,
		.f_ffree = from->f_ffree,
		.f_favail = from->f_ffree,
		.f_fsid = (unsigned long) from->f_fsid.__val[0],
		.f_flag = from->f_flags,
		.f_namemax = from->f_namelen,
	};
}

int sys_statvfs(const char *path, struct statvfs *out) {
	struct statfs buf;
	if(auto ret = sys_statfs(path, &buf); ret != 0) {
		return ret;
	}
	statfs_to_statvfs(&buf, out);
	return 0;
}

int sys_fstatvfs(int fd, struct statvfs *out) {
	struct statfs buf;
	if(auto ret = sys_fstatfs(fd, &buf); ret != 0) {
		return ret;
	}
	statfs_to_statvfs(&buf, out);
	return 0;
}

int sys_sysconf(int num, long *ret) {
	switch(num) {
		case _SC_OPEN_MAX: {
			struct rlimit ru;
			if(int e = sys_getrlimit(RLIMIT_NOFILE, &ru); e) {
				return e;
			}
			*ret = (ru.rlim_cur == RLIM_INFINITY) ? -1 : ru.rlim_cur;
			break;
		}
		case _SC_NPROCESSORS_ONLN: {
			cpu_set_t set;
			CPU_ZERO(&set);
			if(int e = sys_getaffinity(0, sizeof(set), &set); e) {
				return e;
			}
			*ret = CPU_COUNT(&set);
			break;
		}
		case _SC_PHYS_PAGES: {
			struct sysinfo info;
			if(int e = sys_sysinfo(&info); e) {
				return e;
			}
			unsigned unit = (info.mem_unit) ? info.mem_unit : 1;
			*ret = std::min(long((info.totalram * unit) / PAGE_SIZE), LONG_MAX);
			break;
		}
		case _SC_CHILD_MAX: {
			struct rlimit ru;
			if(int e = sys_getrlimit(RLIMIT_NPROC, &ru); e) {
				return e;
			}
			*ret = (ru.rlim_cur == RLIM_INFINITY) ? -1 : ru.rlim_cur;
			break;
		}
		case _SC_LINE_MAX: {
			*ret = -1;
			break;
		}
		default: {
			return EINVAL;
		}
	}

	return 0;
}

int sys_semget(key_t key, int n, int fl, int *id) {
	auto ret = do_syscall(SYS_semget, key, n, fl);
	if(int e = sc_error(ret); e)
		return e;
	*id = sc_int_result<int>(ret);
	return 0;
}

int sys_semctl(int semid, int semnum, int cmd, void *semun, int *out) {
	auto ret = do_syscall(SYS_semctl, semid, semnum, cmd | IPC_64, semun);
	if(int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);
	return 0;
}

int sys_waitid(idtype_t idtype, id_t id, siginfo_t *info, int options) {
	auto ret = do_syscall(SYS_waitid, idtype, id, info, options, 0);
	if(int e = sc_error(ret); e)
		return e;
	return sc_int_result<int>(ret);
}

int sys_clock_set(int clock, time_t secs, long nanos) {
	struct timespec tp{};
	tp.tv_sec = secs;
	tp.tv_nsec = nanos;
	auto ret = do_syscall(SYS_clock_settime, clock, &tp);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#endif // __MLIBC_POSIX_OPTION

#if __MLIBC_LINUX_OPTION

#include <linux/reboot.h>

int sys_reboot(int cmd) {
	auto ret = do_syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, cmd, nullptr);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_getifaddrs(struct ifaddrs **out) {
	*out = nullptr;

	NetlinkHelper nl;
	bool link_ret = nl.send_request(RTM_GETLINK) && nl.recv(&getifaddrs_callback, out);
	__ensure(link_ret);
	bool addr_ret = nl.send_request(RTM_GETADDR) && nl.recv(&getifaddrs_callback, out);
	__ensure(addr_ret);

	return 0;
}

int sys_pidfd_open(pid_t pid, unsigned int flags, int *outfd) {
	auto ret = do_syscall(SYS_pidfd_open, pid, flags);
	if (int e = sc_error(ret); e)
		return e;
	*outfd = sc_int_result<int>(ret);
	return 0;
}

namespace {

char *pidfdGetPidLine = nullptr;
size_t pidfdGetPidLineSize = 0;

} // namespace

int sys_pidfd_getpid(int fd, pid_t *outpid) {
	char *path = nullptr;
	asprintf(&path, "/proc/self/fdinfo/%d", fd);

	FILE *fdinfo = fopen(path, "r");
	if (!fdinfo)
		return errno;

	while (getline(&pidfdGetPidLine, &pidfdGetPidLineSize, fdinfo) != -1) {
		pid_t pid;
		int ret = sscanf(pidfdGetPidLine, "Pid: %d\n", &pid);

		if(ret != 1)
			continue;

		if (pid == 0)
			return EREMOTE;
		else if (pid == -1)
			return ESRCH;

		*outpid = pid;
		return 0;
	}

	free(path);
	return EBADF;
}

int sys_pidfd_send_signal(int pidfd, int sig, siginfo_t *info, unsigned int flags) {
	auto ret = do_syscall(SYS_pidfd_send_signal, pidfd, sig, info, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#endif // __MLIBC_LINUX_OPTION

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

void sys_thread_exit() {
	do_syscall(SYS_exit, 0);
	__builtin_trap();
}

void sys_exit(int status) {
	do_syscall(SYS_exit_group, status);
	__builtin_trap();
}

#endif // MLIBC_BUILDING_RTLD

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

int sys_mkfifoat(int dirfd, const char *path, mode_t mode) {
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

#if __MLIBC_BSD_OPTION
// getloadavg() adapted from musl
int sys_getloadavg(double *samples) {
	struct sysinfo si;
	if (int e = sys_sysinfo(&si); e)
		return e;
	for (int i = 0; i < 3; i++)
		samples[i] = 1.0 / (1 << SI_LOAD_SHIFT) * si.loads[i];
	return 0;
}
#endif /* __MLIBC_BSD_OPTION */

int sys_getrlimit(int resource, struct rlimit *limit) {
	auto ret = do_syscall(SYS_prlimit64, 0, resource, 0, limit);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_setrlimit(int resource, const struct rlimit *limit) {
	auto ret = do_syscall(SYS_prlimit64, 0, resource, limit, 0);
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

int sys_getsid(pid_t pid, pid_t *sid) {
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

int sys_getgroups(size_t size, gid_t *list, int *retval) {
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

int sys_setxattr(const char *path, const char *name, const void *val,
		size_t size, int flags) {
	auto ret = do_syscall(SYS_setxattr, path, name, val, size, flags);
	return sc_error(ret);
}

int sys_lsetxattr(const char *path, const char *name, const void *val,
		size_t size, int flags) {
	auto ret = do_syscall(SYS_lsetxattr, path, name, val, size, flags);
	return sc_error(ret);
}

int sys_fsetxattr(int fd, const char *name, const void *val,
		size_t size, int flags) {
	auto ret = do_syscall(SYS_fsetxattr, fd, name, val, size, flags);
	return sc_error(ret);
}

int sys_getxattr(const char *path, const char *name, void *val, size_t size,
		ssize_t *nread) {
	auto ret = do_syscall(SYS_getxattr, path, name, val, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_lgetxattr(const char *path, const char *name, void *val, size_t size,
		ssize_t *nread) {
	auto ret = do_syscall(SYS_lgetxattr, path, name, val, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_fgetxattr(int fd, const char *name, void *val, size_t size,
		ssize_t *nread) {
	auto ret = do_syscall(SYS_fgetxattr, fd, name, val, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_removexattr(const char *path, const char *name) {
	auto ret = do_syscall(SYS_removexattr, path, name);
	return sc_error(ret);
}

int sys_lremovexattr(const char *path, const char *name) {
	auto ret = do_syscall(SYS_lremovexattr, path, name);
	return sc_error(ret);
}

int sys_fremovexattr(int fd, const char *name) {
	auto ret = do_syscall(SYS_fremovexattr, fd, name);
	return sc_error(ret);
}

int sys_listxattr(const char *path, char *list, size_t size, ssize_t *nread) {
	auto ret = do_syscall(SYS_listxattr, path, list, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_llistxattr(const char *path, char *list, size_t size, ssize_t *nread) {
	auto ret = do_syscall(SYS_llistxattr, path, list, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_flistxattr(int fd, char *list, size_t size, ssize_t *nread) {
	auto ret = do_syscall(SYS_flistxattr, fd, list, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_sigtimedwait(const sigset_t *__restrict set, siginfo_t *__restrict info, const struct timespec *__restrict timeout, int *out_signal) {
	auto ret = do_syscall(SYS_rt_sigtimedwait, set, info, timeout, NSIG / 8);

	if (int e = sc_error(ret); e)
		return e;

	*out_signal = sc_int_result<int>(ret);

	return 0;
}

int sys_sendfile(int outfd, int infd, off_t *offset, size_t count, ssize_t *out) {
	auto ret = do_syscall(SYS_sendfile, outfd, infd, offset, count);
	if(int e = sc_error(ret); e) {
		return e;
	}
	*out = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_syncfs(int fd) {
	auto ret = do_syscall(SYS_syncfs, fd);
	if(int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int sys_name_to_handle_at(int dirfd, const char *pathname, struct file_handle *handle, int *mount_id, int flags) {
	auto ret = do_syscall(SYS_name_to_handle_at, dirfd, pathname, handle, mount_id, flags);
	if (int e = sc_error(ret); e)
		return e;
	return sc_int_result<int>(ret);
}

int sys_splice(int in_fd, off_t *in_off, int out_fd, off_t *out_off, size_t size, unsigned int flags, ssize_t *out) {
	auto ret = do_syscall(SYS_copy_file_range, in_fd, in_off, out_fd, out_off, size, flags);
	if(int e = sc_error(ret); e) {
		return e;
	}
	*out = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_unshare(int flags) {
	auto ret = do_syscall(SYS_unshare, flags);
	if(int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int sys_setns(int fd, int nstype) {
	auto ret = do_syscall(SYS_setns, fd, nstype);
	if(int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int sys_setgroups(size_t size, const gid_t *list) {
	auto ret = do_syscall(SYS_setgroups, size, list);
	if(int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

#if __MLIBC_BSD_OPTION
int sys_brk(void **out) {
	auto ret = do_syscall(SYS_brk, 0);
	if(int e = sc_error(ret); e) {
		return e;
	}

	*out = (void *) sc_int_result<uintptr_t>(ret);
	return 0;
}
#endif // __MLIBC_BSD_OPTION

#if __MLIBC_GLIBC_OPTION

int sys_personality(unsigned long persona, int *out) {
	auto ret = do_syscall(SYS_personality, persona);

	if(int e = sc_error(ret); e) {
		return e;
	}

	*out = sc_int_result<int>(ret);
	return 0;
}

int sys_ioperm(unsigned long int from, unsigned long int num, int turn_on) {
#if defined(SYS_ioperm)
	auto ret = do_syscall(SYS_ioperm, from, num, turn_on);

	if(int e = sc_error(ret); e) {
		return e;
	}

	return 0;
#else
	(void) from;
	(void) num;
	(void) turn_on;
	return ENOSYS;
#endif
}

int sys_iopl(int level) {
#if defined(SYS_iopl)
	auto ret = do_syscall(SYS_iopl, level);

	if(int e = sc_error(ret); e) {
		return e;
	}

	return 0;
#else
	(void) level;
	return ENOSYS;
#endif
}

#endif // __MLIBC_GLIBC_OPTION

#if __MLIBC_POSIX_OPTION

int sys_shmat(void **seg_start, int shmid, const void *shmaddr, int shmflg) {
	auto ret = do_syscall(SYS_shmat, shmid, shmaddr, shmflg);
	if (int e = sc_error(ret); e)
		return e;
	*seg_start = sc_ptr_result<void>(ret);
	return 0;
}

int sys_shmctl(int *idx, int shmid, int cmd, struct shmid_ds *buf) {
	auto ret = do_syscall(SYS_shmctl, shmid, cmd, buf);
	if (int e = sc_error(ret); e)
		return e;
	*idx = sc_int_result<int>(ret);
	return 0;
}

int sys_shmdt(const void *shmaddr) {
	auto ret = do_syscall(SYS_shmdt, shmaddr);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_shmget(int *shm_id, key_t key, size_t size, int shmflg) {
	auto ret = do_syscall(SYS_shmget, key, size, shmflg);
	if (int e = sc_error(ret); e)
		return e;
	*shm_id = sc_int_result<int>(ret);
	return 0;
}

#endif // __MLIBC_POSIX_OPTION

} // namespace mlibc
