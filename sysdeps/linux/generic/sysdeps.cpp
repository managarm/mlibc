#include <asm/ioctls.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include <type_traits>

#include <mlibc-config.h>
#include <asm/ioctls.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include <abi-bits/fcntl.h>
#include <abi-bits/ioctls.h>
#include <abi-bits/socklen_t.h>
#include <bits/ensure.h>
#include <frg/scope_exit.hpp>
#include <limits.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/dlapi.hpp>
#include <mlibc/thread-entry.hpp>

#if __MLIBC_POSIX_OPTION
#include <net/if.h>
#include <pthread.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/user.h>
#endif // __MLIBC_POSIX_OPTION

#if __MLIBC_LINUX_OPTION
#include <abi-bits/statx.h>
#endif // __MLIBC_LINUX_OPTION

#include <sys/syscall.h>
#include "cxx-syscall.hpp"

#if __MLIBC_LINUX_OPTION && !defined(MLIBC_BUILDING_RTLD)
#include "generic-helpers/netlink.hpp"
#endif

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

void Sysdeps<LibcLog>::operator()(const char *message) {
	size_t n = 0;
	while(message[n])
		n++;
	do_syscall(SYS_write, 2, message, n);
	char lf = '\n';
	do_syscall(SYS_write, 2, &lf, 1);
}

void Sysdeps<LibcPanic>::operator()() {
	__builtin_trap();
}

pid_t Sysdeps<FutexTid>::operator()() {
	auto ret = do_syscall(SYS_gettid);
	// gettid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	return sysdep<VmMap>(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
			-1, 0, pointer);
}
int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
	return sysdep<VmUnmap>(pointer, size);
}

int Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
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

	return 0;
}

int Sysdeps<VmMap>::operator()(void *hint, size_t size, int prot, int flags,
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

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	auto ret = do_syscall(SYS_munmap, pointer, size);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
	auto ret = do_syscall(SYS_mprotect, pointer, size, prot);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
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
#elif defined(__m68k__)
extern "C" void *__m68k_read_tp() {
	auto ret = do_syscall(__NR_get_thread_area, 0);
	return (void *)ret;
}
#endif

int Sysdeps<TcbSet>::operator()(void *pointer) {
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

#if defined(__riscv)
int Sysdeps<RiscvHwprobe>::operator()(struct riscv_hwprobe *pairs, size_t pair_count, size_t cpusetsize, cpu_set_t *cpus, unsigned int flags) {
	auto ret = do_syscall(SYS_riscv_hwprobe, pairs, pair_count, cpusetsize, cpus, flags);
	if(int e = sc_error(ret); e)
		return -e;
	return sc_int_result<int>(ret);
}
#endif // defined(__riscv)

[[noreturn]]
void Sysdeps<Exit>::operator()(int status) {
	do_syscall(SYS_exit_group, status);
	__builtin_trap();
}

[[noreturn]]
void Sysdeps<ThreadExit>::operator()() {
	do_syscall(SYS_exit, 0);
	__builtin_trap();
}

#if __MLIBC_POSIX_OPTION && !MLIBC_BUILDING_RTLD
int Sysdeps<Clone>::operator()(void *tcb, pid_t *pid_out, void *stack) {
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

	auto ret = __mlibc_spawn_thread(flags, stack, pid_out, nullptr, tcb);
	if (ret < 0)
		return ret;

	return 0;
}
#endif // __MLIBC_POSIX_OPTION

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
	auto ret = do_cp_syscall(SYS_futex, pointer, FUTEX_WAIT, expected, time);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
	auto ret = do_syscall(SYS_futex, pointer, FUTEX_WAKE, all ? INT_MAX : 1);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Open>::operator()(const char *path, int flags, mode_t mode, int *fd) {
	auto ret = do_cp_syscall(SYS_openat, AT_FDCWD, path, flags, mode);
	if(int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Flock>::operator()(int fd, int options) {
	auto ret = do_syscall(SYS_flock, fd, options);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<OpenDir>::operator()(const char *path, int *fd) {
	return sysdep<Open>(path, O_DIRECTORY, 0, fd);
}

int Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	auto ret = do_syscall(SYS_getdents64, handle, buffer, max_size);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Read>::operator()(int fd, void *buffer, size_t size, ssize_t *bytes_read) {
	auto ret = do_cp_syscall(SYS_read, fd, buffer, size);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Write>::operator()(int fd, const void *buffer, size_t size, ssize_t *bytes_written) {
	auto ret = do_cp_syscall(SYS_write, fd, buffer, size);
	if(int e = sc_error(ret); e)
		return e;
	if(bytes_written)
		*bytes_written = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Pread>::operator()(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	auto ret = do_syscall(SYS_pread64, fd, buf, n, off);
	if (int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	auto ret = do_syscall(SYS_lseek, fd, offset, whence);
	if(int e = sc_error(ret); e)
		return e;
	*new_offset = sc_int_result<off_t>(ret);
	return 0;
}

int Sysdeps<Close>::operator()(int fd) {
	auto ret = do_cp_syscall(SYS_close, fd);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

#if __MLIBC_POSIX_OPTION && !MLIBC_BUILDING_RTLD
#define VDSO_SYMBOL(name) (__dlapi_vdsosym(name, nullptr))
#else
#define VDSO_SYMBOL(name) ((void*)nullptr)
#endif

static auto vdso_clock_gettime = reinterpret_cast<int (*)(clockid_t, struct timespec *)>(
	VDSO_SYMBOL("__vdso_clock_gettime")
);

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
	struct timespec tp = {};

	if (vdso_clock_gettime) {
		if (int e = vdso_clock_gettime(clock, &tp); e)
			return e;
	} else {
		auto ret = do_syscall(SYS_clock_gettime, clock, &tp);
		if (int e = sc_error(ret); e)
			return e;
	}

	*secs = tp.tv_sec;
	*nanos = tp.tv_nsec;
	return 0;
}

int Sysdeps<ClockSet>::operator()(int clock, time_t secs, long nanos) {
	struct timespec tp{};
	tp.tv_sec = secs;
	tp.tv_nsec = nanos;
	auto ret = do_syscall(SYS_clock_settime, clock, &tp);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<ClockGetres>::operator()(int clock, time_t *secs, long *nanos) {
	struct timespec tp = {};
	auto ret = do_syscall(SYS_clock_getres, clock, &tp);
	if (int e = sc_error(ret); e)
		return e;
	*secs = tp.tv_sec;
	*nanos = tp.tv_nsec;
	return 0;
}

int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
	__ensure(*nanos < 1'000'000'000);

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

int Sysdeps<Isatty>::operator()(int fd) {
	unsigned short winsizeHack[4];
	auto ret = do_syscall(SYS_ioctl, fd, 0x5413 /* TIOCGWINSZ */, &winsizeHack);
	if (int e = sc_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<Rmdir>::operator()(const char *path) {
	auto ret = do_syscall(SYS_unlinkat, AT_FDCWD, path, AT_REMOVEDIR);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Unlinkat>::operator()(int dfd, const char *path, int flags) {
	auto ret = do_syscall(SYS_unlinkat, dfd, path, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Rename>::operator()(const char *old_path, const char *new_path) {
#ifdef SYS_renameat2
	auto ret = do_syscall(SYS_renameat2, AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
#else
	auto ret = do_syscall(SYS_renameat, AT_FDCWD, old_path, AT_FDCWD, new_path);
#endif /* defined(SYS_renameat2) */
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<FdToPath>::operator()(int fd, char **out) {
	frg::string path{getAllocator()};
	frg::output_to(path) << frg::fmt("/proc/self/fd/{}", fd);
	*out = path.data();
	path.detach();
	return 0;
}

int Sysdeps<Sigprocmask>::operator()(int how, const sigset_t *set, sigset_t *old) {
	auto ret = do_syscall(SYS_rt_sigprocmask, how, set, old, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#if !MLIBC_BUILDING_RTLD
extern "C" void __mlibc_signal_restore(void);
extern "C" void __mlibc_signal_restore_rt(void);

int Sysdeps<Sigaction>::operator()(int signum, const struct sigaction *act,
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
		&kernel_act : nullptr, oldact ?
		&kernel_oldact : nullptr, sizeof(kernel_act.mask));
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
#endif // !MLIBC_BUILDING_RTLD

int Sysdeps<Fork>::operator()(pid_t *child) {
	auto ret = do_syscall(SYS_clone, SIGCHLD, 0);
	if (int e = sc_error(ret); e)
			return e;
	*child = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	auto ret = do_syscall(SYS_wait4, pid, status, flags, ru);
	if (int e = sc_error(ret); e)
			return e;
	*ret_pid = sc_int_result<pid_t>(ret);
	return 0;
}

int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	auto ret = do_syscall(SYS_execve, path, argv, envp);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

void Sysdeps<Yield>::operator()() {
	do_syscall(SYS_sched_yield);
}

pid_t Sysdeps<GetPid>::operator()() {
	auto ret = do_syscall(SYS_getpid);
	// getpid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

int Sysdeps<Kill>::operator()(pid_t pid, int sig) {
	auto ret = do_syscall(SYS_kill, pid, sig);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#if !MLIBC_BUILDING_RTLD
#if __MLIBC_POSIX_OPTION
int Sysdeps<Readv>::operator()(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read) {
	auto ret = do_cp_syscall(SYS_readv, fd, iovs, iovc);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Writev>::operator()(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_written) {
	auto ret = do_cp_syscall(SYS_writev, fd, iovs, iovc);
	if(int e = sc_error(ret); e)
		return e;
	if(bytes_written)
		*bytes_written = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Pwrite>::operator()(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
	auto ret = do_syscall(SYS_pwrite64, fd, buf, n, off);
	if (int e = sc_error(ret); e)
		return e;
	*bytes_written = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Access>::operator()(const char *path, int mode) {
	auto ret = do_syscall(SYS_faccessat, AT_FDCWD, path, mode, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Faccessat>::operator()(int dirfd, const char *pathname, int mode, int flags) {
	auto ret = do_syscall(SYS_faccessat2, dirfd, pathname, mode, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
	__ensure(!flags);
	auto ret = do_cp_syscall(SYS_dup, fd);
	if (int e = sc_error(ret); e)
		return e;
	*newfd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
	auto ret = do_cp_syscall(SYS_dup3, fd, newfd, flags);
	if(int e = sc_error(ret); e)
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
#if __INTPTR_WIDTH__ == 32
		.__f_unused = 0,
#endif
		.f_flag = from->f_flags,
		.f_namemax = from->f_namelen,
		.f_spare = { 0 },
	};
}

int Sysdeps<Statvfs>::operator()(const char *path, struct statvfs *out) {
	struct statfs buf;
	if(auto ret = sysdep<Statfs>(path, &buf); ret != 0) {
		return ret;
	}
	statfs_to_statvfs(&buf, out);
	return 0;
}

int Sysdeps<Fstatvfs>::operator()(int fd, struct statvfs *out) {
	struct statfs buf;
	if(auto ret = sysdep<Fstatfs>(fd, &buf); ret != 0) {
		return ret;
	}
	statfs_to_statvfs(&buf, out);
	return 0;
}

int Sysdeps<Readlink>::operator()(const char *path, void *buf, size_t bufsiz, ssize_t *len) {
	auto ret = do_syscall(SYS_readlinkat, AT_FDCWD, path, buf, bufsiz);
	if (int e = sc_error(ret); e)
		return e;
	*len = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Readlinkat>::operator()(int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length) {
	auto ret = do_syscall(SYS_readlinkat, dirfd, path, buffer, max_size);
	if (int e = sc_error(ret); e)
		return e;
	*length = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Truncate>::operator()(const char *path, off_t length) {
	auto ret = do_syscall(SYS_truncate, path, length);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Ftruncate>::operator()(int fd, size_t size) {
	auto ret = do_syscall(SYS_ftruncate, fd, size);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Fallocate>::operator()(int fd, off_t offset, size_t size) {
	auto ret = do_syscall(SYS_fallocate, fd, 0, offset, size);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Openat>::operator()(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	auto ret = do_syscall(SYS_openat, dirfd, path, flags, mode);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Socket>::operator()(int domain, int type, int protocol, int *fd) {
	auto ret = do_syscall(SYS_socket, domain, type, protocol);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<MsgSend>::operator()(int sockfd, const struct msghdr *msg, int flags, ssize_t *length) {
	// Work around ABI mismatches: POSIX requires us to expose some members of a particular type,
	// and Linux kernel ABI is using larger types. Our fix is to add padding members to preserve
	// struct layout and zero them out here.
#if __INTPTR_WIDTH__ == 64
	const_cast<msghdr *>(msg)->__pad0 = 0;
	const_cast<msghdr *>(msg)->__pad1 = 0;

	for (auto cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg))
		cmsg->__pad = 0;
#endif /* __INTPTR_WIDTH__ == 64 */

	auto ret = do_cp_syscall(SYS_sendmsg, sockfd, msg, flags);
	if (int e = sc_error(ret); e)
		return e;
	*length = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Sendto>::operator()(int fd, const void *buffer, size_t size, int flags, const sockaddr *sock_addr, socklen_t addr_length, ssize_t *length) {
	auto ret = do_cp_syscall(SYS_sendto, fd, buffer, size, flags, sock_addr, addr_length);
	if(int e = sc_error(ret); e) {
		return e;
	}
	*length = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<MsgRecv>::operator()(int sockfd, struct msghdr *msg, int flags, ssize_t *length) {
	// Work around ABI mismatches: POSIX requires us to expose some members of a particular type,
	// and Linux kernel ABI is using larger types. Our fix is to add padding members to preserve
	// struct layout and zero them out here.
#if __INTPTR_WIDTH__ == 64
	const_cast<msghdr *>(msg)->__pad0 = 0;
	const_cast<msghdr *>(msg)->__pad1 = 0;
#endif /* __INTPTR_WIDTH__ == 64 */

	auto ret = do_cp_syscall(SYS_recvmsg, sockfd, msg, flags);
	if (int e = sc_error(ret); e)
			return e;
	*length = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Recvfrom>::operator()(int fd, void *buffer, size_t size, int flags, struct sockaddr *sock_addr, socklen_t *addr_length, ssize_t *length) {
	auto ret = do_cp_syscall(SYS_recvfrom, fd, buffer, size, flags, sock_addr, addr_length);
	if(int e = sc_error(ret); e) {
		return e;
	}
	*length = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Listen>::operator()(int fd, int backlog) {
	auto ret = do_syscall(SYS_listen, fd, backlog, 0, 0, 0, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

gid_t Sysdeps<GetGid>::operator()() {
	auto ret = do_syscall(SYS_getgid);
	// getgid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

gid_t Sysdeps<GetEgid>::operator()() {
	auto ret = do_syscall(SYS_getegid);
	// getegid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

uid_t Sysdeps<GetUid>::operator()() {
	auto ret = do_syscall(SYS_getuid);
	// getuid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

uid_t Sysdeps<GetEuid>::operator()() {
	auto ret = do_syscall(SYS_geteuid);
	// geteuid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

pid_t Sysdeps<GetTid>::operator()() {
	auto ret = do_syscall(SYS_gettid);
	// gettid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

pid_t Sysdeps<GetPpid>::operator()() {
	auto ret = do_syscall(SYS_getppid);
	// getppid() always succeeds.
	return sc_int_result<pid_t>(ret);
}

int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *out) {
	auto ret = do_syscall(SYS_getpgid, pid);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<pid_t>(ret);
	return 0;
}

int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
	auto ret = do_syscall(SYS_setpgid, pid, pgid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetSid>::operator()(pid_t pid, pid_t *sid) {
	auto ret = do_syscall(SYS_getsid, pid);
	if (int e = sc_error(ret); e)
		return e;
	*sid = sc_int_result<pid_t>(ret);
	return 0;
}

int Sysdeps<SetUid>::operator()(uid_t uid) {
	auto ret = do_syscall(SYS_setuid, uid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetEuid>::operator()(uid_t euid) {
	return sysdep<SetResuid>(-1, euid, -1);
}

int Sysdeps<SetGid>::operator()(gid_t gid) {
	auto ret = do_syscall(SYS_setgid, gid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetEgid>::operator()(gid_t egid) {
	return sysdep<SetResgid>(-1, egid, -1);
}

int Sysdeps<GetGroups>::operator()(size_t size, gid_t *list, int *retval) {
	auto ret = do_syscall(SYS_getgroups, size, list);
	if (int e = sc_error(ret); e)
		return e;
	*retval = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Fexecve>::operator()(int fd, char *const argv[], char *const envp[]) {
	auto ret = do_syscall(SYS_execveat, fd, "", argv, envp, AT_EMPTY_PATH);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Pselect>::operator()(int nfds, fd_set *readfds, fd_set *writefds,
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

int Sysdeps<GetRusage>::operator()(int scope, struct rusage *usage) {
	auto ret = do_syscall(SYS_getrusage, scope, usage);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetRlimit>::operator()(int resource, struct rlimit *limit) {
	auto ret = do_syscall(SYS_prlimit64, 0, resource, 0, limit);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetRlimit>::operator()(int resource, const struct rlimit *limit) {
	auto ret = do_syscall(SYS_prlimit64, 0, resource, limit, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetPriority>::operator()(int which, id_t who, int *value) {
	auto ret = do_syscall(SYS_getpriority, which, who);
	if (int e = sc_error(ret); e) {
		return e;
	}
	*value = 20 - sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<SetPriority>::operator()(int which, id_t who, int prio) {
	auto ret = do_syscall(SYS_setpriority, which, who, prio);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetSchedparam>::operator()(void *tcb, int *policy, struct sched_param *param) {
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

int Sysdeps<SetSchedparam>::operator()(void *tcb, int policy, const struct sched_param *param) {
	auto t = reinterpret_cast<Tcb *>(tcb);

	if(!t->tid) {
		return ESRCH;
	}

	auto ret = do_syscall(SYS_sched_setscheduler, t->tid, policy, param);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetScheduler>::operator()(pid_t pid, int *sched) {
	auto ret = do_syscall(SYS_sched_getscheduler, pid);
	if (int e = sc_error(ret); e)
		return e;
	*sched = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<SetScheduler>::operator()(pid_t pid, int policy, const struct sched_param *param) {
	auto ret = do_syscall(SYS_sched_setscheduler, pid, policy, param);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetParam>::operator()(pid_t pid, struct sched_param *param) {
	auto ret = do_syscall(SYS_sched_getparam, pid, param);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetParam>::operator()(pid_t pid, const struct sched_param *param) {
	auto ret = do_syscall(SYS_sched_setparam, pid, param);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetMaxPriority>::operator()(int policy, int *out) {
	auto ret = do_syscall(SYS_sched_get_priority_max, policy);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<GetMinPriority>::operator()(int policy, int *out) {
	auto ret = do_syscall(SYS_sched_get_priority_min, policy);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);

	return 0;
}

int Sysdeps<GetCwd>::operator()(char *buf, size_t size) {
	auto ret = do_syscall(SYS_getcwd, buf, size);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int Sysdeps<Chdir>::operator()(const char *path) {
	auto ret = do_syscall(SYS_chdir, path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Fchdir>::operator()(int fd) {
	auto ret = do_syscall(SYS_fchdir, fd);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
	auto ret = do_syscall(SYS_mkdirat, AT_FDCWD, path, mode);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}


int Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode) {
	auto ret = do_syscall(SYS_mkdirat, dirfd, path, mode);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
#ifdef SYS_link
	auto ret = do_syscall(SYS_link, old_path, new_path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
#else
	return sysdep<Linkat>(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
#endif
}

int Sysdeps<Linkat>::operator()(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	auto ret = do_syscall(SYS_linkat, olddirfd, old_path, newdirfd, new_path, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
	auto ret = do_syscall(SYS_symlinkat, target_path, AT_FDCWD, link_path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
	auto ret = do_syscall(SYS_symlinkat, target_path, dirfd, link_path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Renameat>::operator()(int old_dirfd, const char *old_path, int new_dirfd, const char *new_path) {
#ifdef SYS_renameat2
	auto ret = do_syscall(SYS_renameat2, old_dirfd, old_path, new_dirfd, new_path, 0);
#else
	auto ret = do_syscall(SYS_renameat, old_dirfd, old_path, new_dirfd, new_path);
#endif /* defined(SYS_renameat2) */
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Fcntl>::operator()(int fd, int cmd, va_list args, int *result) {
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

int Sysdeps<Ttyname>::operator()(int fd, char *buf, size_t size) {
	if (int e = sysdep<Isatty>(fd); e)
		return e;

	frg::string<MemoryAllocator> str {getAllocator()};
	frg::output_to(str) << frg::fmt("/proc/self/fd/{}", fd);

	ssize_t l;
	if (int e = sysdep<Readlink>(str.data(), buf, size, &l); e)
		return e;

	if ((size_t)l >= size)
		return ERANGE;

	buf[l] = '\0';
	struct stat st1, st2;

	if (int e1 = sysdep<Stat>(fsfd_target::path, -1, buf, 0, &st1),
		e2 = sysdep<Stat>(fsfd_target::fd, fd, "", 0, &st2);
		e1 || e2)
		return e1 ? e1 : e2;

	if (st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino)
		return ENODEV;

	return 0;
}

int Sysdeps<Fadvise>::operator()(int fd, off_t offset, off_t length, int advice) {
	auto ret = do_syscall(SYS_fadvise64, fd, offset, length, advice);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

void Sysdeps<Sync>::operator()() {
	do_syscall(SYS_sync);
}

int Sysdeps<Fsync>::operator()(int fd) {
	auto ret = do_syscall(SYS_fsync, fd);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Fdatasync>::operator()(int fd) {
	auto ret = do_syscall(SYS_fdatasync, fd);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Chmod>::operator()(const char *pathname, mode_t mode) {
	auto ret = do_cp_syscall(SYS_fchmodat, AT_FDCWD, pathname, mode);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Fchmod>::operator()(int fd, mode_t mode) {
	auto ret = do_cp_syscall(SYS_fchmod, fd, mode);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Fchmodat>::operator()(int fd, const char *pathname, mode_t mode, int flags) {
	auto ret = do_cp_syscall(SYS_fchmodat2, fd, pathname, mode, flags);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Utimensat>::operator()(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	auto ret = do_cp_syscall(SYS_utimensat, dirfd, pathname, times, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Mlock>::operator()(const void *addr, size_t length) {
	auto ret = do_syscall(SYS_mlock, addr, length);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Munlock>::operator()(const void *addr, size_t length) {
	auto ret = do_syscall(SYS_munlock, addr, length);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Mlockall>::operator()(int flags) {
	auto ret = do_syscall(SYS_mlockall, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Munlockall>::operator()() {
	auto ret = do_syscall(SYS_munlockall);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Mincore>::operator()(void *addr, size_t length, unsigned char *vec) {
	auto ret = do_syscall(SYS_mincore, addr, length, vec);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<VmRemap>::operator()(void *pointer, size_t size, size_t new_size, void **window) {
	auto ret = do_syscall(SYS_mremap, pointer, size, new_size, MREMAP_MAYMOVE);
	// TODO: musl fixes up EPERM errors from the kernel.
	if(int e = sc_error(ret); e)
		return e;
	*window = sc_ptr_result<void>(ret);
	return 0;
}

int Sysdeps<SetSid>::operator()(pid_t *sid) {
	auto ret = do_syscall(SYS_setsid);
	if (int e = sc_error(ret); e)
		return e;
	*sid = sc_int_result<pid_t>(ret);
	return 0;
}

int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr) {
	auto ret = do_syscall(SYS_ioctl, fd, TCGETS, attr);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcsetattr>::operator()(int fd, int optional_action, const struct termios *attr) {
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

int Sysdeps<Tcsendbreak>::operator()(int fd, int) {
	auto ret = do_syscall(SYS_ioctl, fd, TCSBRK, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcflow>::operator()(int fd, int action) {
	auto ret = do_syscall(SYS_ioctl, fd, TCXONC, action);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcflush>::operator()(int fd, int queue) {
	auto ret = do_syscall(SYS_ioctl, fd, TCFLSH, queue);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcdrain>::operator()(int fd) {
	auto ret = do_syscall(SYS_ioctl, fd, TCSBRK, 1);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcgetwinsize>::operator()(int fd, struct winsize *winsz) {
	auto ret = do_syscall(SYS_ioctl, fd, TIOCGWINSZ, winsz);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Tcsetwinsize>::operator()(int fd, const struct winsize *winsz) {
	auto ret = do_syscall(SYS_ioctl, fd, TIOCSWINSZ, winsz);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Pipe>::operator()(int *fds, int flags) {
	auto ret = do_syscall(SYS_pipe2, fds, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Socketpair>::operator()(int domain, int type_and_flags, int proto, int *fds) {
	auto ret = do_syscall(SYS_socketpair, domain, type_and_flags, proto, fds, 0, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Poll>::operator()(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	struct timespec tm;
	tm.tv_sec = timeout / 1000;
	tm.tv_nsec = timeout % 1000 * 1000000;
	auto ret = do_syscall(SYS_ppoll, fds, count, timeout >= 0 ? &tm : nullptr, 0, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	*num_events = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Ppoll>::operator()(struct pollfd *fds, nfds_t count, const struct timespec *ts, const sigset_t *mask, int *num_events) {
	auto ret = do_syscall(SYS_ppoll, fds, count, ts, mask, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	*num_events = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
	auto ret = do_syscall(SYS_ioctl, fd, request, arg);
	if (int e = sc_error(ret); e)
		return e;
	if (result)
		*result = sc_int_result<unsigned long>(ret);
	return 0;
}

int Sysdeps<GetSockopt>::operator()(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
	auto ret = do_syscall(SYS_getsockopt, fd, layer, number, buffer, size, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetSockopt>::operator()(int fd, int layer, int number, const void *buffer, socklen_t size) {
	auto ret = do_syscall(SYS_setsockopt, fd, layer, number, buffer, size, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Shutdown>::operator()(int sockfd, int how) {
	auto ret = do_syscall(SYS_shutdown, sockfd, how);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int Sysdeps<Sockatmark>::operator()(int sockfd, int *out) {
	auto ret = do_syscall(SYS_ioctl, sockfd, SIOCATMARK, out);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Sigtimedwait>::operator()(const sigset_t *__restrict set, siginfo_t *__restrict info, const struct timespec *__restrict timeout, int *out_signal) {
	auto ret = do_syscall(SYS_rt_sigtimedwait, set, info, timeout, NSIG / 8);

	if (int e = sc_error(ret); e)
		return e;

	*out_signal = sc_int_result<int>(ret);

	return 0;
}

int Sysdeps<Accept>::operator()(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	auto ret = do_syscall(SYS_accept4, fd, addr_ptr, addr_length, flags);
	if (int e = sc_error(ret); e)
		return e;
	*newfd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	auto ret = do_syscall(SYS_bind, fd, addr_ptr, addr_length, 0, 0, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Connect>::operator()(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	auto ret = do_cp_syscall(SYS_connect, sockfd, addr, addrlen);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Sockname>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	auto ret = do_syscall(SYS_getsockname, fd, addr_ptr, &max_addr_length);
	if (int e = sc_error(ret); e)
		return e;
	*actual_length = max_addr_length;
	return 0;
}

int Sysdeps<Peername>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	auto ret = do_syscall(SYS_getpeername, fd, addr_ptr, &max_addr_length);
	if (int e = sc_error(ret); e)
		return e;
	*actual_length = max_addr_length;
	return 0;
}

int Sysdeps<GetHostname>::operator()(char *buf, size_t bufsize) {
	struct utsname uname_buf;
	if (auto e = sysdep<Uname>(&uname_buf); e)
		return e;

	auto node_len = strlen(uname_buf.nodename);
	if (node_len >= bufsize)
		return ENAMETOOLONG;

	memcpy(buf, uname_buf.nodename, node_len);
	buf[node_len] = '\0';
	return 0;
}

int Sysdeps<SetHostname>::operator()(const char *buffer, size_t bufsize) {
	auto ret = do_syscall(SYS_sethostname, buffer, bufsize);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Mkfifoat>::operator()(int dirfd, const char *path, mode_t mode) {
	return sysdep<Mknodat>(dirfd, path, mode | S_IFIFO, 0);
}

int Sysdeps<GetEntropy>::operator()(void *buffer, size_t length) {
	auto ret = do_syscall(SYS_getrandom, buffer, length, 0);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Mknodat>::operator()(int dirfd, const char *path, int mode, int dev) {
	auto ret = do_syscall(SYS_mknodat, dirfd, path, mode, dev);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Umask>::operator()(mode_t mode, mode_t *old) {
	auto ret = do_syscall(SYS_umask, mode);
	if (int e = sc_error(ret); e)
		return e;
	*old = sc_int_result<mode_t>(ret);
	return 0;
}

extern "C" const char __mlibc_syscall_begin[1];
extern "C" const char __mlibc_syscall_end[1];

#if defined(__riscv)
// Disable UBSan here to work around qemu-user misaligning ucontext_t.
// https://github.com/qemu/qemu/blob/2bf40d0841b942e7ba12953d515e62a436f0af84/linux-user/riscv/signal.c#L68-L69
[[gnu::no_sanitize("undefined")]]
#endif
int Sysdeps<BeforeCancellableSyscall>::operator()(ucontext_t *uct) {
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

int Sysdeps<Tgkill>::operator()(int tgid, int tid, int sig) {
	auto ret = do_syscall(SYS_tgkill, tgid, tid, sig);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Fchownat>::operator()(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
	auto ret = do_cp_syscall(SYS_fchownat, dirfd, pathname, owner, group, flags);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Sigaltstack>::operator()(const stack_t *ss, stack_t *oss) {
	auto ret = do_syscall(SYS_sigaltstack, ss, oss);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Sigsuspend>::operator()(const sigset_t *set) {
	auto ret = do_syscall(SYS_rt_sigsuspend, set, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Sigpending>::operator()(sigset_t *set) {
	auto ret = do_syscall(SYS_rt_sigpending, set, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#if !defined(MLIBC_BUILDING_RTLD)
int Sysdeps<Sigqueue>::operator()(pid_t pid, int sig, const union sigval val) {
	siginfo_t si;
	memset(&si, 0, sizeof(si));
	si.si_signo = sig;
	si.si_code = SI_QUEUE;
	si.si_value = val;
	si.si_uid = Sysdeps<GetUid>::operator()();
	si.si_pid = Sysdeps<GetPid>::operator()();

	auto ret = do_syscall(SYS_rt_sigqueueinfo, pid, sig, &si);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}
#endif // !defined(MLIBC_BUILDING_RTLD)

int Sysdeps<SetGroups>::operator()(size_t size, const gid_t *list) {
	auto ret = do_syscall(SYS_setgroups, size, list);
	if(int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int Sysdeps<MemfdCreate>::operator()(const char *name, int flags, int *fd) {
	auto ret = do_syscall(SYS_memfd_create, name, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Madvise>::operator()(void *addr, size_t length, int advice) {
	auto ret = do_syscall(SYS_madvise, addr, length, advice);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<PosixMadvise>::operator()(void *addr, size_t length, int advice) {
	if(advice == POSIX_MADV_DONTNEED) {
		// POSIX_MADV_DONTNEED is a no-op in both glibc and musl.
		return 0;
	}
	switch(advice) {
	case POSIX_MADV_NORMAL:
		advice = MADV_NORMAL;
		break;
	case POSIX_MADV_RANDOM:
		advice = MADV_RANDOM;
		break;
	case POSIX_MADV_SEQUENTIAL:
		advice = MADV_SEQUENTIAL;
		break;
	case POSIX_MADV_WILLNEED:
		advice = MADV_WILLNEED;
		break;
	default:
		return EINVAL;
	}
	return sysdep<Madvise>(addr, length, advice);
}

int Sysdeps<Msync>::operator()(void *addr, size_t length, int flags) {
	auto ret = do_syscall(SYS_msync, addr, length, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetItimer>::operator()(int which, const struct itimerval *new_value, struct itimerval *old_value) {
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

namespace {

bool timerThreadInit = false;

struct PosixTimerContext {
	int setupSem = 0;
	int workerSem = 0;
	sigevent *sigev;
};

void timer_handle(int, siginfo_t *, void *) {
}

void *timer_setup(void *arg) {
	auto ctx = reinterpret_cast<PosixTimerContext *>(arg);

	sigset_t set{};
	sigaddset(&set, SIGTIMER);

	// wait for parent setup to be complete
	while(__atomic_load_n(&ctx->setupSem, __ATOMIC_RELAXED) == 0);
	pthread_testcancel();

	// copy out the function and argument, as the lifetime of the context ends with
	// incrementing workerSem
	auto notify = ctx->sigev->sigev_notify_function;
	union sigval val = ctx->sigev->sigev_value;

	// notify the parent that the context can be dropped
	__atomic_store_n(&ctx->workerSem, 1, __ATOMIC_RELEASE);

	siginfo_t si{};
	int signo;

	while(true) {
		while(sysdep<Sigtimedwait>(&set, &si, nullptr, &signo));
		if(si.si_code == SI_TIMER && signo == SIGTIMER)
			notify(val);
		pthread_testcancel();
	}

	return nullptr;
}

} // namespace

int Sysdeps<TimerCreate>::operator()(clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res) {
	struct linux_uapi_sigevent ksev;
	struct linux_uapi_sigevent *ksevp = nullptr;
	int timer_id;

	switch(evp ? evp->sigev_notify : SIGEV_SIGNAL) {
		case SIGEV_NONE:
		case SIGEV_SIGNAL:
		case SIGEV_THREAD_ID: {
			if(evp) {
				ksev.sigev_value = evp->sigev_value;
				ksev.sigev_signo = evp->sigev_signo;
				ksev.sigev_notify = evp->sigev_notify;
				ksev.sigev_tid = (evp && evp->sigev_notify == SIGEV_THREAD_ID) ? evp->sigev_notify_thread_id : 0;
				ksevp = &ksev;
			}

			auto ret = do_syscall(SYS_timer_create, clk, ksevp, &timer_id);
			if (int e = sc_error(ret); e) {
				return e;
			}
			*res = (void *) (intptr_t) timer_id;
			break;
		}
		case SIGEV_THREAD: {
			if(!timerThreadInit) {
				struct sigaction sa{};
				sa.sa_flags = SA_SIGINFO | SA_RESTART;
				sa.sa_sigaction = timer_handle;
				sysdep<Sigaction>(SIGTIMER, &sa, nullptr);
				timerThreadInit = true;
			}

			pthread_attr_t attr;
			if(evp->sigev_notify_attributes)
				attr = *evp->sigev_notify_attributes;
			else
				pthread_attr_init(&attr);

			int ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
			if(ret)
				return ret;

			PosixTimerContext context{};
			context.sigev = evp;

			// mask for all signals except the libc-reserved RT signal range
			sigset_t mask = {
				#if ULONG_MAX == 0xFFFF'FFFF
					0x7FFF'FFFF, 0xFFFF'FFFC
				#else
					0xFFFF'FFFC'7FFF'FFFF
				#endif
			};
			// but also mask SIGTIMER
			sigaddset(&mask, SIGTIMER);
			sigset_t original_set;
			do_syscall(SYS_rt_sigprocmask, SIG_BLOCK, &mask, &original_set, _NSIG / 8);

			pthread_t pthread;
			ret = pthread_create(&pthread, &attr, timer_setup, &context);

			// restore previous signal mask
			do_syscall(SYS_rt_sigprocmask, SIG_SETMASK, &original_set, 0, _NSIG / 8);

			if(ret)
				return ret;

			auto tid = reinterpret_cast<Tcb*>(pthread)->tid;

			linux_uapi_sigevent sigev{
				.sigev_value = { .sival_ptr = nullptr },
				.sigev_signo = SIGTIMER,
				.sigev_notify = SIGEV_THREAD_ID,
				.sigev_tid = tid,
			};
			ksevp = &sigev;

			auto syscallret = do_syscall(SYS_timer_create, clk, ksevp, &timer_id);
			if (int e = sc_error(syscallret); e) {
				pthread_cancel(pthread);
				__atomic_store_n(&context.setupSem, 1, __ATOMIC_RELEASE);
				return e;
			}

			// notify worker that setup is complete
			__atomic_store_n(&context.setupSem, 1, __ATOMIC_RELEASE);
			// await worker setup to let the context go out of scope
			while(__atomic_load_n(&context.workerSem, __ATOMIC_RELAXED) == 0);

			*res = (void *) (intptr_t) timer_id;
			break;
		}
		default:
			return EINVAL;
	}

	return 0;
}

int Sysdeps<TimerSettime>::operator()(timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old) {
	auto ret = do_syscall(SYS_timer_settime, t, flags, val, old);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int Sysdeps<TimerGettime>::operator()(timer_t t, struct itimerspec *val) {
	auto ret = do_syscall(SYS_timer_gettime, t, val);
	if (int e = sc_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<TimerDelete>::operator()(timer_t t) {
	__ensure((intptr_t) t >= 0);
	auto ret = do_syscall(SYS_timer_delete, t);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int Sysdeps<TimerGetoverrun>::operator()(timer_t t, int *out) {
	auto ret = do_syscall(SYS_timer_getoverrun, t);
	if (int e = sc_error(ret); e)
		return e;

	*out = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Times>::operator()(struct tms *tms, clock_t *out) {
	auto ret = do_syscall(SYS_times, tms);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<long>(ret);
	return 0;
}

int Sysdeps<Uname>::operator()(struct utsname *buf) {
	auto ret = do_syscall(SYS_uname, buf);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Pause>::operator()() {
#ifdef SYS_pause
	auto ret = do_syscall(SYS_pause);
#else
	auto ret = do_syscall(SYS_ppoll, 0, 0, 0, 0);
#endif
	if (int e = sc_error(ret); e)
		return e;
	return EINTR;
}

int Sysdeps<SetResuid>::operator()(uid_t ruid, uid_t euid, uid_t suid) {
	auto ret = do_syscall(SYS_setresuid, ruid, euid, suid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetResgid>::operator()(gid_t rgid, gid_t egid, gid_t sgid) {
	auto ret = do_syscall(SYS_setresgid, rgid, egid, sgid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetResuid>::operator()(uid_t *ruid, uid_t *euid, uid_t *suid) {
	auto ret = do_syscall(SYS_getresuid, ruid, euid, suid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetResgid>::operator()(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	auto ret = do_syscall(SYS_getresgid, rgid, egid, sgid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetReuid>::operator()(uid_t ruid, uid_t euid) {
	auto ret = do_syscall(SYS_setreuid, ruid, euid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetRegid>::operator()(gid_t rgid, gid_t egid) {
	auto ret = do_syscall(SYS_setregid, rgid, egid);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetLoginR>::operator()(char *name, size_t name_len) {
	int cs = 0;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	int fd;
	if(sysdep<Open>("/proc/self/loginuid", O_RDONLY, 0, &fd))
		return EINVAL;


	char buf[12];
	ssize_t bytes_read = 0;
	if(int e = sysdep<Read>(fd, buf, sizeof(buf), &bytes_read); e || !bytes_read)
		return EINVAL;

	char *uid_end = nullptr;
	auto uid = strtol(buf, &uid_end, 10);
	if (uid_end == buf || uid < 0)
		return EINVAL;

	sysdep<Close>(fd);

	pthread_setcancelstate(cs, nullptr);

	struct passwd pwd;
	struct passwd *tpwd;
	char pwbuf[1024];
	if (getpwuid_r(uid, &pwd, pwbuf, sizeof(pwbuf), &tpwd) || tpwd == NULL)
		return EINVAL;

	size_t needed = strlen(pwd.pw_name) + 1;
	if (needed > name_len)
		return ERANGE;

	memcpy(name, pwd.pw_name, needed);

	return 0;
}

int Sysdeps<IfIndextoname>::operator()(unsigned int index, char *name) {
	int fd = 0;
	int r = sysdep<Socket>(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if(r)
		return r;

	struct ifreq ifr;
	ifr.ifr_ifindex = index;

	int ret = sysdep<Ioctl>(fd, SIOCGIFNAME, &ifr, nullptr);
	close(fd);

	if(ret) {
		if(ret == ENODEV)
			return ENXIO;
		return ret;
	}

	strncpy(name, ifr.ifr_name, IF_NAMESIZE);

	return 0;
}

int Sysdeps<IfNametoindex>::operator()(const char *name, unsigned int *ret) {
	int fd = 0;
	int r = sysdep<Socket>(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, AF_UNSPEC, &fd);

	if(r)
		return r;

	struct ifreq ifr;
	strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);

	r = sysdep<Ioctl>(fd, SIOCGIFINDEX, &ifr, nullptr);
	close(fd);

	if(r) {
		return r;
	}

	*ret = ifr.ifr_ifindex;

	return 0;
}

int Sysdeps<Ptsname>::operator()(int fd, char *buffer, size_t length) {
	int index;
	if(int e = sysdep<Ioctl>(fd, TIOCGPTN, &index, nullptr); e)
		return e;
	if((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
		return ERANGE;
	}
	return 0;
}

int Sysdeps<Unlockpt>::operator()(int fd) {
	int unlock = 0;

	if(int e = sysdep<Ioctl>(fd, TIOCSPTLCK, &unlock, nullptr); e)
		return e;

	return 0;
}

#if !MLIBC_BUILDING_RTLD
int Sysdeps<ThreadSetname>::operator()(void *tcb, const char *name) {
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
	if(int e = sysdep<Open>(path, O_WRONLY, 0, &fd); e) {
		return e;
	}

	if(int e = sysdep<Write>(fd, name, strlen(name) + 1, nullptr)) {
		return e;
	}

	sysdep<Close>(fd);

	pthread_setcancelstate(cs, nullptr);

	return 0;
}
#endif

int Sysdeps<ThreadGetname>::operator()(void *tcb, char *name, size_t size) {
	auto t = reinterpret_cast<Tcb *>(tcb);
	char *path;
	int cs = 0;
	ssize_t real_size = 0;

	if(asprintf(&path, "/proc/self/task/%d/comm", t->tid) < 0) {
		return ENOMEM;
	}

	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

	int fd;
	if(int e = sysdep<Open>(path, O_RDONLY | O_CLOEXEC, 0, &fd); e) {
		return e;
	}

	if(int e = sysdep<Read>(fd, name, size, &real_size)) {
		return e;
	}

	name[real_size - 1] = 0;
	sysdep<Close>(fd);

	pthread_setcancelstate(cs, nullptr);

	if(static_cast<ssize_t>(size) <= real_size) {
		return ERANGE;
	}

	return 0;
}

int Sysdeps<Sysconf>::operator()(int num, long *ret) {
	switch(num) {
		case _SC_OPEN_MAX: {
			struct rlimit ru;
			if(int e = sysdep<GetRlimit>(RLIMIT_NOFILE, &ru); e) {
				return e;
			}
			*ret = (ru.rlim_cur == RLIM_INFINITY) ? -1 : ru.rlim_cur;
			break;
		}
		case _SC_NPROCESSORS_CONF:
		case _SC_NPROCESSORS_ONLN: {
			/* TODO: glibc seems to try to read sysfs files first:
			 * `/sys/devices/system/cpu/{online,possible}`, failing that `/proc/stat`.
			 */
			cpu_set_t set;
			CPU_ZERO(&set);
			if(int e = sysdep<GetAffinity>(0, sizeof(set), &set); e) {
				return e;
			}
			*ret = CPU_COUNT(&set);
			break;
		}
		case _SC_PHYS_PAGES: {
			struct sysinfo info;
			if(int e = sysdep<Sysinfo>(&info); e) {
				return e;
			}
			unsigned unit = (info.mem_unit) ? info.mem_unit : 1;
			*ret = std::min(long((info.totalram * unit) / PAGE_SIZE), LONG_MAX);
			break;
		}
		case _SC_CHILD_MAX: {
			struct rlimit ru;
			if(int e = sysdep<GetRlimit>(RLIMIT_NPROC, &ru); e) {
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

int Sysdeps<Semget>::operator()(key_t key, int n, int fl, int *id) {
	auto ret = do_syscall(SYS_semget, key, n, fl);
	if(int e = sc_error(ret); e)
		return e;
	*id = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Semctl>::operator()(int semid, int semnum, int cmd, void *semun, int *out) {
	auto ret = do_syscall(SYS_semctl, semid, semnum, cmd | IPC_64, semun);
	if(int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<GetAffinity>::operator()(pid_t pid, size_t cpusetsize, cpu_set_t *mask) {
	auto ret = do_syscall(SYS_sched_getaffinity, pid, cpusetsize, mask);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<GetThreadaffinity>::operator()(pid_t tid, size_t cpusetsize, cpu_set_t *mask) {
	auto ret = do_syscall(SYS_sched_getaffinity, tid, cpusetsize, mask);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetAffinity>::operator()(pid_t pid, size_t cpusetsize, const cpu_set_t *mask) {
	auto ret = do_syscall(SYS_sched_setaffinity, pid, cpusetsize, mask);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<SetThreadaffinity>::operator()(pid_t tid, size_t cpusetsize, const cpu_set_t *mask) {
	auto ret = do_syscall(SYS_sched_setaffinity, tid, cpusetsize, mask);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Waitid>::operator()(idtype_t idtype, id_t id, siginfo_t *info, int options) {
	auto ret = do_syscall(SYS_waitid, idtype, id, info, options, 0);
	if(int e = sc_error(ret); e)
		return e;
	return sc_int_result<int>(ret);
}

int Sysdeps<NameToHandleAt>::operator()(int dirfd, const char *pathname, struct file_handle *handle, int *mount_id, int flags) {
	auto ret = do_syscall(SYS_name_to_handle_at, dirfd, pathname, handle, mount_id, flags);
	if (int e = sc_error(ret); e)
		return e;
	return sc_int_result<int>(ret);
}

int Sysdeps<Splice>::operator()(int in_fd, off_t *in_off, int out_fd, off_t *out_off, size_t size, unsigned int flags, ssize_t *out) {
	auto ret = do_syscall(SYS_splice, in_fd, in_off, out_fd, out_off, size, flags);
	if(int e = sc_error(ret); e) {
		return e;
	}
	*out = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Shmat>::operator()(void **seg_start, int shmid, const void *shmaddr, int shmflg) {
	auto ret = do_syscall(SYS_shmat, shmid, shmaddr, shmflg);
	if (int e = sc_error(ret); e)
		return e;
	*seg_start = sc_ptr_result<void>(ret);
	return 0;
}

int Sysdeps<Shmctl>::operator()(int *idx, int shmid, int cmd, struct shmid_ds *buf) {
	auto ret = do_syscall(SYS_shmctl, shmid, cmd, buf);
	if (int e = sc_error(ret); e)
		return e;
	*idx = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Shmdt>::operator()(const void *shmaddr) {
	auto ret = do_syscall(SYS_shmdt, shmaddr);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Shmget>::operator()(int *shm_id, key_t key, size_t size, int shmflg) {
	auto ret = do_syscall(SYS_shmget, key, size, shmflg);
	if (int e = sc_error(ret); e)
		return e;
	*shm_id = sc_int_result<int>(ret);
	return 0;
}

#if !defined(MLIBC_BUILDING_RTLD)
int Sysdeps<InetConfigured>::operator()(bool *ipv4, bool *ipv6) {
	struct context {
		bool *ipv4;
		bool *ipv6;
	} context = {
		.ipv4 = ipv4,
		.ipv6 = ipv6
	};

	NetlinkHelper nl;
	if (!nl.send_request(RTM_GETADDR)) {
		*ipv4 = false;
		*ipv6 = false;
		return 0;
	}

	auto ret = nl.recv([](void *data, const nlmsghdr *hdr) {
		if (hdr->nlmsg_type == RTM_NEWADDR && hdr->nlmsg_len >= sizeof(struct ifaddrmsg)) {
			const struct ifaddrmsg *ifaddr = reinterpret_cast<const struct ifaddrmsg *>(NLMSG_DATA(hdr));
			struct context *ctx = reinterpret_cast<struct context *>(data);

			char name[IF_NAMESIZE];
			auto interfaceNameResult = sysdep<IfIndextoname>(ifaddr->ifa_index, name);

			if (interfaceNameResult || !strncmp(name, "lo", IF_NAMESIZE))
				return;

			if (ifaddr->ifa_family == AF_INET)
				*ctx->ipv4 = true;
			else if (ifaddr->ifa_family == AF_INET6)
				*ctx->ipv6 = true;
		}
	}, &context);

	if (!ret) {
		*ipv4 = false;
		*ipv6 = false;
		return 0;
	}

	return 0;
}
#endif // !defined(MLIBC_BUILDING_RTLD)

// the first argument of the get/set priority calls is a PRIO_PROCESS constant.
// the actual macro is not used at the moment because of a wrong #define
// FIXME once the abi fix PR is merged
int Sysdeps<Nice>::operator()(int increment, int *new_nice) {
	int current;
	if (int e = sysdep<GetPriority>(0, 0, &current); e)
		return e;

	if (increment == 0) {
		*new_nice = current;
		return 0;
	}

	// the system call silently clamps the value to the nice range
	if (int e = sysdep<SetPriority>(0, 0, current + increment); e)
		return e;

	if (int e = sysdep<GetPriority>(0, 0, &current); e)
		return e;

	// NOTE: according to man 2 getpriority, the internal priority values in linux are
	// in the range 40..1. So we have to convert it.
	*new_nice = 20 - current;
	return 0;
}

int Sysdeps<Msgctl>::operator()(int q, int cmd, struct msqid_ds *buf) {
#if __INTPTR_WIDTH__ == 32
	cmd |= IPC_64;
#endif
	auto ret = do_syscall(SYS_msgctl, q, cmd, buf);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Msgget>::operator()(key_t k, int flag, int *out) {
	auto ret = do_syscall(SYS_msgget, k, flag);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Msgrcv>::operator()(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg, ssize_t *out) {
	auto ret = do_syscall(SYS_msgrcv, msqid, msgp, msgsz, msgtyp, msgflg);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Msgsnd>::operator()(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	auto ret = do_syscall(SYS_msgsnd, msqid, msgp, msgsz, msgflg);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<MqOpen>::operator()(const char *name, int oflag, mode_t mode, mq_attr *attr, mqd_t *out) {
	auto ret = do_syscall(SYS_mq_open, name, oflag, mode, attr);
	if (int e = sc_error(ret); e)
		return e;
	if (out)
		*out = sc_int_result<mqd_t>(ret);
	return 0;
}

int Sysdeps<MqUnlink>::operator()(const char *name) {
	auto ret = do_syscall(SYS_mq_unlink, name);
	if (int e = sc_error(ret); e) {
		// We need to translate Linux's EPERM to the POSIX-mandated EACCESS on permission denial.
		if (e == EPERM)
			return EACCES;
		return e;
	}
	return 0;
}

int Sysdeps<MqReceive>::operator()(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
	auto ret = do_syscall(SYS_mq_timedreceive, mqdes, msg_ptr, msg_len, msg_prio, nullptr);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<MqGetAttr>::operator()(mqd_t mqdes, mq_attr *mqstat) {
	auto ret = do_syscall(SYS_mq_getsetattr, mqdes, nullptr, mqstat);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<MqSetAttr>::operator()(mqd_t mqdes, const mq_attr *mqstat, mq_attr *omqstat) {
	auto ret = do_syscall(SYS_mq_getsetattr, mqdes, mqstat, omqstat);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}
#endif // __MLIBC_POSIX_OPTION

#if __MLIBC_LINUX_OPTION
int Sysdeps<InotifyCreate>::operator()(int flags, int *fd) {
	auto ret = do_syscall(SYS_inotify_init1, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<InotifyAddWatch>::operator()(int ifd, const char *path, uint32_t mask, int *wd) {
	auto ret = do_syscall(SYS_inotify_add_watch, ifd, path, mask);
	if (int e = sc_error(ret); e)
		return e;
	*wd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<InotifyRmWatch>::operator()(int ifd, int wd) {
	auto ret = do_syscall(SYS_inotify_rm_watch, ifd, wd);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Mount>::operator()(const char *source, const char *target,
	const char *fstype, unsigned long flags, const void *data) {
	auto ret = do_syscall(SYS_mount, source, target, fstype, flags, data);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Umount2>::operator()(const char *target, int flags) {
	auto ret = do_syscall(SYS_umount2, target, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Ptrace>::operator()(long req, pid_t pid, void *addr, void *data, long *out) {
	auto ret = do_syscall(SYS_ptrace, req, pid, addr, data);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<long>(ret);
	return 0;
}

int Sysdeps<Capget>::operator()(cap_user_header_t hdrp, cap_user_data_t datap) {
	auto ret = do_syscall(SYS_capget, hdrp, datap);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Capset>::operator()(cap_user_header_t hdrp, const cap_user_data_t datap) {
	auto ret = do_syscall(SYS_capset, hdrp, datap);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Prctl>::operator()(int op, va_list ap, int *out) {
	unsigned long x[4];
	for(int i = 0; i < 4; i++)
		x[i] = va_arg(ap, unsigned long);

	auto ret = do_syscall(SYS_prctl, op, x[0], x[1], x[2], x[3]);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<InitModule>::operator()(void *module, unsigned long length, const char *args) {
	auto ret = do_syscall(SYS_init_module, module, length, args);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<DeleteModule>::operator()(const char *name, unsigned flags) {
	auto ret = do_syscall(SYS_delete_module, name, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Klogctl>::operator()(int type, char *bufp, int len, int *out) {
	auto ret = do_syscall(SYS_syslog, type, bufp, len);
	if (int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Getcpu>::operator()(int *cpu) {
	auto ret = do_syscall(SYS_getcpu, cpu, NULL, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Sysinfo>::operator()(struct sysinfo *info) {
	auto ret = do_syscall(SYS_sysinfo, info);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Swapon>::operator()(const char *path, int flags) {
	auto ret = do_syscall(SYS_swapon, path, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Swapoff>::operator()(const char *path) {
	auto ret = do_syscall(SYS_swapoff, path);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Setxattr>::operator()(const char *path, const char *name, const void *val,
		size_t size, int flags) {
	auto ret = do_syscall(SYS_setxattr, path, name, val, size, flags);
	return sc_error(ret);
}

int Sysdeps<Lsetxattr>::operator()(const char *path, const char *name, const void *val,
		size_t size, int flags) {
	auto ret = do_syscall(SYS_lsetxattr, path, name, val, size, flags);
	return sc_error(ret);
}

int Sysdeps<Fsetxattr>::operator()(int fd, const char *name, const void *val,
		size_t size, int flags) {
	auto ret = do_syscall(SYS_fsetxattr, fd, name, val, size, flags);
	return sc_error(ret);
}

int Sysdeps<Getxattr>::operator()(const char *path, const char *name, void *val, size_t size,
		ssize_t *nread) {
	auto ret = do_syscall(SYS_getxattr, path, name, val, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Lgetxattr>::operator()(const char *path, const char *name, void *val, size_t size,
		ssize_t *nread) {
	auto ret = do_syscall(SYS_lgetxattr, path, name, val, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Fgetxattr>::operator()(int fd, const char *name, void *val, size_t size,
		ssize_t *nread) {
	auto ret = do_syscall(SYS_fgetxattr, fd, name, val, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Listxattr>::operator()(const char *path, char *list, size_t size, ssize_t *nread) {
	auto ret = do_syscall(SYS_listxattr, path, list, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Llistxattr>::operator()(const char *path, char *list, size_t size, ssize_t *nread) {
	auto ret = do_syscall(SYS_llistxattr, path, list, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Flistxattr>::operator()(int fd, char *list, size_t size, ssize_t *nread) {
	auto ret = do_syscall(SYS_flistxattr, fd, list, size);
	if (int e = sc_error(ret); e) {
		return e;
	}

	*nread = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Removexattr>::operator()(const char *path, const char *name) {
	auto ret = do_syscall(SYS_removexattr, path, name);
	return sc_error(ret);
}

int Sysdeps<Lremovexattr>::operator()(const char *path, const char *name) {
	auto ret = do_syscall(SYS_lremovexattr, path, name);
	return sc_error(ret);
}

int Sysdeps<Fremovexattr>::operator()(int fd, const char *name) {
	auto ret = do_syscall(SYS_fremovexattr, fd, name);
	return sc_error(ret);
}

int Sysdeps<Statfs>::operator()(const char *path, struct statfs *buf) {
	auto ret = do_cp_syscall(SYS_statfs, path, buf);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Fstatfs>::operator()(int fd, struct statfs *buf) {
	auto ret = do_cp_syscall(SYS_fstatfs, fd, buf);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

static_assert(sizeof(struct statx) == 0x100); // Linux kernel requires it to be precisely 256 bytes.

int Sysdeps<Statx>::operator()(int dirfd, const char *path, int flags, unsigned int mask, struct statx *statxbuf) {
	auto ret = do_cp_syscall(SYS_statx, dirfd, path, flags, mask, statxbuf);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#if !defined(MLIBC_BUILDING_RTLD)
int Sysdeps<Getifaddrs>::operator()(struct ifaddrs **out) {
	*out = nullptr;

	NetlinkHelper nl;
	bool link_ret = nl.send_request(RTM_GETLINK) && nl.recv(&getifaddrs_callback, out);
	__ensure(link_ret);
	bool addr_ret = nl.send_request(RTM_GETADDR) && nl.recv(&getifaddrs_callback, out);
	__ensure(addr_ret);

	return 0;
}
#endif // !defined(MLIBC_BUILDING_RTLD)

int Sysdeps<Sendfile>::operator()(int outfd, int infd, off_t *offset, size_t count, ssize_t *out) {
	auto ret = do_syscall(SYS_sendfile, outfd, infd, offset, count);
	if(int e = sc_error(ret); e) {
		return e;
	}
	*out = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Syncfs>::operator()(int fd) {
	auto ret = do_syscall(SYS_syncfs, fd);
	if(int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int Sysdeps<Unshare>::operator()(int flags) {
	auto ret = do_syscall(SYS_unshare, flags);
	if(int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int Sysdeps<SetNs>::operator()(int fd, int nstype) {
	auto ret = do_syscall(SYS_setns, fd, nstype);
	if(int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int Sysdeps<PidfdOpen>::operator()(pid_t pid, unsigned int flags, int *outfd) {
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

int Sysdeps<PidfdGetpid>::operator()(int fd, pid_t *outpid) {
	char *path = nullptr;
	asprintf(&path, "/proc/self/fdinfo/%d", fd);
	frg::scope_exit freePath{[&] {
		free(path);
	}};

	FILE *fdinfo = fopen(path, "r");
	if (!fdinfo)
		return errno;

	frg::scope_exit closeFile{[&] {
		fclose(fdinfo);
	}};

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

	return EBADF;
}

int Sysdeps<PidfdSendSignal>::operator()(int pidfd, int sig, siginfo_t *info, unsigned int flags) {
	auto ret = do_syscall(SYS_pidfd_send_signal, pidfd, sig, info, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<ProcessVmReadv>::operator()(pid_t pid,
		const struct iovec *local_iov, unsigned long liovcnt,
		const struct iovec *remote_iov, unsigned long riovcnt,
		unsigned long flags, ssize_t *out) {
	auto ret = do_syscall(SYS_process_vm_readv, pid, local_iov, liovcnt,
			remote_iov, riovcnt, flags);
	if(int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<ProcessVmWritev>::operator()(pid_t pid,
		const struct iovec *local_iov, unsigned long liovcnt,
		const struct iovec *remote_iov, unsigned long riovcnt,
		unsigned long flags, ssize_t *out) {
	auto ret = do_syscall(SYS_process_vm_writev, pid, local_iov, liovcnt,
			remote_iov, riovcnt, flags);
	if(int e = sc_error(ret); e)
		return e;
	*out = sc_int_result<ssize_t>(ret);
	return 0;
}

int Sysdeps<Fsopen>::operator()(const char *fsname, unsigned int flags, int *outfd) {
	auto ret = do_syscall(SYS_fsopen, fsname, flags);
	if (int e = sc_error(ret); e)
		return e;
	*outfd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Fsmount>::operator()(int fsfd, unsigned int flags, unsigned int mountflags, int *outfd) {
	if (!fsfd)
		return EINVAL;
	auto ret = do_syscall(SYS_fsmount, fsfd, flags, mountflags);
	if (int e = sc_error(ret); e)
		return e;
	*outfd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Fsconfig>::operator()(int fd, unsigned int cmd, const char *key, const void *val, int aux) {
	auto ret = do_syscall(SYS_fsconfig, fd, cmd, key, val, aux);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<MoveMount>::operator()(int from_dirfd, const char *from_path, int to_dirfd, const char *to_path, unsigned int flags) {
	auto ret = do_syscall(SYS_move_mount, from_dirfd, from_path, to_dirfd, to_path, flags);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<OpenTree>::operator()(int dirfd, const char *path, unsigned int flags, int *out_fd) {
	auto ret = do_syscall(SYS_open_tree, dirfd, path, flags);
	if (int e = sc_error(ret); e)
		return e;
	*out_fd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<CopyFileRange>::operator()(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t count, unsigned int flags, ssize_t *bytes_copied) {
	auto ret = do_syscall(SYS_copy_file_range, fd_in, off_in, fd_out, off_out, count, flags);
	if (int e = sc_error(ret); e)
		return e;
	*bytes_copied = sc_int_result<ssize_t>(ret);
	return 0;
}
#endif // __MLIBC_LINUX_OPTION

#if __MLIBC_LINUX_EPOLL_OPTION

int Sysdeps<EpollCreate>::operator()(int flags, int *fd) {
	auto ret = do_syscall(SYS_epoll_create1, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<EpollCtl>::operator()(int epfd, int mode, int fd, struct epoll_event *ev) {
	auto ret = do_syscall(SYS_epoll_ctl, epfd, mode, fd, ev);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<EpollPwait>::operator()(int epfd, struct epoll_event *ev, int n, int timeout, const sigset_t *sigmask, int *raised) {
	auto ret = do_syscall(SYS_epoll_pwait, epfd, ev, n, timeout, sigmask, NSIG / 8);
	if (int e = sc_error(ret); e)
		return e;
	*raised = sc_int_result<int>(ret);
	return 0;
}

#endif // __MLIBC_LINUX_EPOLL_OPTION

#if __MLIBC_LINUX_TIMERFD_OPTION

int Sysdeps<TimerfdCreate>::operator()(int clockid, int flags, int *fd) {
	auto ret = do_syscall(SYS_timerfd_create, clockid, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<TimerfdSettime>::operator()(int fd, int flags, const struct itimerspec *value, struct itimerspec *oldvalue) {
	auto ret = do_syscall(SYS_timerfd_settime, fd, flags, value, oldvalue);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<TimerfdGettime>::operator()(int fd, struct itimerspec *its) {
	auto ret = do_syscall(SYS_timerfd_gettime, fd, its);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#endif // __MLIBC_LINUX_TIMERFD_OPTION

#if __MLIBC_LINUX_SIGNALFD_OPTION

int Sysdeps<SignalfdCreate>::operator()(const sigset_t *masks, int flags, int *fd) {
	auto ret = do_syscall(SYS_signalfd4, *fd, masks, NSIG / 8, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

#endif // __MLIBC_LINUX_SIGNALFD_OPTION

#if __MLIBC_LINUX_EVENTFD_OPTION

int Sysdeps<EventfdCreate>::operator()(unsigned int initval, int flags, int *fd) {
	auto ret = do_syscall(SYS_eventfd2, initval, flags);
	if (int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

#endif // __MLIBC_LINUX_EVENTFD_OPTION

#if __MLIBC_LINUX_REBOOT_OPTION

#include <linux/reboot.h>

int Sysdeps<Reboot>::operator()(int cmd) {
	auto ret = do_syscall(SYS_reboot, LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2, cmd, nullptr);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#endif // __MLIBC_LINUX_REBOOT_OPTION

#if __MLIBC_LINUX_WRAPPERS_OPTION

int Sysdeps<RtSigqueueinfo>::operator()(pid_t tgid, int sig, siginfo_t *uinfo) {
	auto ret = do_syscall(SYS_rt_sigqueueinfo, tgid, sig, uinfo);
	if(int e = sc_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<RtTgSigqueueinfo>::operator()(pid_t tgid, pid_t tid, int sig, siginfo_t *uinfo) {
	auto ret = do_syscall(SYS_rt_tgsigqueueinfo, tgid, tid, sig, uinfo);
	if(int e = sc_error(ret); e)
		return e;

	return 0;
}

#endif // __MLIBC_LINUX_WRAPPERS_OPTION

#if __MLIBC_BSD_OPTION

int Sysdeps<Brk>::operator()(void **out) {
	auto ret = do_syscall(SYS_brk, 0);
	if(int e = sc_error(ret); e) {
		return e;
	}

	*out = (void *) sc_int_result<uintptr_t>(ret);
	return 0;
}

// getloadavg() adapted from musl
int Sysdeps<GetLoadavg>::operator()(double *samples) {
	struct sysinfo si;
	if (int e = sysdep<Sysinfo>(&si); e)
		return e;
	for (int i = 0; i < 3; i++)
		samples[i] = 1.0 / (1 << SI_LOAD_SHIFT) * si.loads[i];
	return 0;
}

#endif // __MLIBC_BSD_OPTION

#if __MLIBC_GLIBC_OPTION

int Sysdeps<Personality>::operator()(unsigned long persona, int *out) {
	auto ret = do_syscall(SYS_personality, persona);

	if(int e = sc_error(ret); e) {
		return e;
	}

	*out = sc_int_result<int>(ret);
	return 0;
}

int Sysdeps<Ioperm>::operator()(unsigned long int from, unsigned long int num, int turn_on) {
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

int Sysdeps<Iopl>::operator()(int level) {
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

#endif // !MLIBC_BUILDING_RTLD

} // namespace mlibc
