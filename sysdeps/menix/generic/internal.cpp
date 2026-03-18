#include <abi-bits/fcntl.h>
#include <abi-bits/pid_t.h>
#include <menix/archctl.hpp>
#include <menix/syscall.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

namespace mlibc {

void Sysdeps<LibcLog>::operator()(const char *message) {
	menix_syscall(SYSCALL_SYSLOG, LOG_INFO, (size_t)message, strlen(message));
}

[[noreturn]] void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("mlibc panic!");
	menix_syscall(SYSCALL_EXIT, 1);
	__builtin_unreachable();
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
#if defined(__x86_64__)
	return menix_syscall(SYSCALL_ARCHCTL, ARCHCTL_SET_FSBASE, (size_t)pointer).error;
#elif defined(__aarch64__)
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb) - 0x10;
	asm volatile("msr tpidr_el0, %0" ::"r"(thread_data));
	return 0;
#elif defined(__riscv)
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile("mv tp, %0" ::"r"(thread_data));
	return 0;
#elif defined(__loongarch64)
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile("move $tp, %0" ::"r"(thread_data));
	return 0;
#else
#error "Unsupported architecture!"
#endif
}

int Sysdeps<FutexTid>::operator()() { return menix_syscall(SYSCALL_GETTID).value; }

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
	return menix_syscall(SYSCALL_FUTEX_WAIT, (size_t)pointer, expected, (size_t)time).error;
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool) {
	return menix_syscall(SYSCALL_FUTEX_WAKE, (size_t)pointer).error;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	auto r = menix_syscall(
	    SYSCALL_MMAP, 0, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0
	);
	if (r.error)
		return r.error;
	*pointer = (void *)r.value;
	return 0;
}

int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
	return menix_syscall(SYSCALL_MUNMAP, (size_t)pointer, size).error;
}

int Sysdeps<Openat>::operator()(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	auto r = menix_syscall(SYSCALL_OPENAT, dirfd, (size_t)path, flags, mode);
	if (r.error)
		return r.error;
	*fd = (int)r.value;
	return 0;
}

int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd) {
	return sysdep<Openat>(AT_FDCWD, pathname, flags, mode, fd);
}

int Sysdeps<Read>::operator()(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	auto r = menix_syscall(SYSCALL_READ, fd, (size_t)buf, count);
	if (r.error)
		return r.error;
	*bytes_read = (ssize_t)r.value;
	return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	auto r = menix_syscall(SYSCALL_SEEK, fd, offset, whence);
	if (r.error)
		return r.error;
	*new_offset = r.value;
	return 0;
}

int Sysdeps<Close>::operator()(int fd) { return menix_syscall(SYSCALL_CLOSE, fd).error; }

int Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	switch (fsfdt) {
		case fsfd_target::path:
			return menix_syscall(SYSCALL_FSTATAT, AT_FDCWD, (size_t)path, (size_t)statbuf, flags)
			    .error;
		case fsfd_target::fd_path:
			return menix_syscall(SYSCALL_FSTATAT, fd, (size_t)path, (size_t)statbuf, flags).error;
		case fsfd_target::fd:
			return menix_syscall(SYSCALL_FSTAT, fd, (size_t)statbuf).error;
		default:
			return EINVAL;
	}
}

int Sysdeps<VmMap>::operator()(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	auto r = menix_syscall(SYSCALL_MMAP, (size_t)hint, size, prot, flags, fd, offset);
	if (r.error)
		return r.error;
	*window = (void *)r.value;
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	return menix_syscall(SYSCALL_MUNMAP, (size_t)pointer, size).error;
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
	return menix_syscall(SYSCALL_MPROTECT, (size_t)pointer, size, prot).error;
}

} // namespace mlibc
