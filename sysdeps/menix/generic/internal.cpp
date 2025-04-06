#include <abi-bits/fcntl.h>
#include <abi-bits/pid_t.h>
#include <menix/archctl.hpp>
#include <menix/syscall.hpp>
#include <mlibc/internal-sysdeps.hpp>
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <mlibc/tcb.hpp>

namespace mlibc {

void sys_libc_log(const char *message) {
	menix_syscall(SYSCALL_WRITE, 1, (size_t)message, strlen(message));
	menix_syscall(SYSCALL_WRITE, 1, (size_t)"\n", 1);
}

[[noreturn]] void sys_libc_panic() {
	sys_libc_log("mlibc panic!");
	menix_syscall(SYSCALL_EXIT, 1);
	__builtin_unreachable();
}

int sys_tcb_set(void *pointer) {
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

int sys_futex_tid() { return menix_syscall(SYSCALL_GETTID).error; }

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	return menix_syscall(SYSCALL_FUTEX_WAIT, (size_t)pointer, expected, (size_t)time).error;
}

int sys_futex_wake(int *pointer) {
	return menix_syscall(SYSCALL_FUTEX_WAKE, (size_t)pointer).error;
}

int sys_anon_allocate(size_t size, void **pointer) {
	auto r = menix_syscall(
	    SYSCALL_MMAP, 0, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0
	);
	if (r.error)
		return r.error;
	*pointer = (void *)r.value;
	return 0;
}

int sys_anon_free(void *pointer, size_t size) {
	return menix_syscall(SYSCALL_MUNMAP, (size_t)pointer, size).error;
}

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	auto r = menix_syscall(SYSCALL_OPENAT, dirfd, (size_t)path, flags, mode);
	if (r.error)
		return r.error;
	*fd = (int)r.value;
	return 0;
}

int sys_open(const char *pathname, int flags, mode_t mode, int *fd) {
	return sys_openat(AT_FDCWD, pathname, flags, mode, fd);
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	auto r = menix_syscall(SYSCALL_READ, fd, (size_t)buf, count);
	if (r.error)
		return r.error;
	*bytes_read = (ssize_t)r.value;
	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	auto r = menix_syscall(SYSCALL_SEEK, fd, offset, whence);
	if (r.error)
		return r.error;
	*new_offset = r.value;
	return 0;
}

int sys_close(int fd) { return menix_syscall(SYSCALL_CLOSE, fd).error; }

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	(void)fsfdt;
	return menix_syscall(SYSCALL_FSTAT, fd, (size_t)path, flags, (size_t)statbuf).error;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	auto r = menix_syscall(SYSCALL_MMAP, (size_t)hint, size, prot, flags, fd, offset);
	if (r.error)
		return r.error;
	*window = (void *)r.value;
	return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
	return menix_syscall(SYSCALL_MUNMAP, (size_t)pointer, size).error;
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
	return menix_syscall(SYSCALL_MPROTECT, (size_t)pointer, size, prot).error;
}

} // namespace mlibc
