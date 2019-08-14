#include <errno.h>
#include <type_traits>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>
#include "cxx-syscall.hpp"

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

#define NR_read 0
#define NR_write 1
#define NR_open 2
#define NR_close 3
#define NR_lseek 8
#define NR_mmap 9
#define NR_exit 60
#define NR_arch_prctl 158

#define ARCH_SET_FS	0x1002

namespace mlibc {

void sys_libc_log(const char *message) {
	size_t n = 0;
	while(message[n])
		n++;
	do_syscall(NR_write, 1, message, n);
	char lf = '\n';
	do_syscall(NR_write, 1, &lf, 1);
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
	auto ret = do_syscall(NR_open, path, flags, 0);
	if(int e = sc_error(ret); e)
		return e;
	*fd = sc_int_result<int>(ret);
	return 0;
}

int sys_close(int fd) {
	auto ret = do_syscall(NR_close, fd);
	if(int e = sc_error(ret); e)
		return e;
	return 0;
}

int sys_read(int fd, void *buffer, size_t size, ssize_t *bytes_read) {
	auto ret = do_syscall(NR_read, fd, buffer, size);
	if(int e = sc_error(ret); e)
		return e;
	*bytes_read = sc_int_result<ssize_t>(ret);
	return 0;
}

int sys_write(int fd, const void *buffer, size_t size, ssize_t *bytes_written) {
	auto ret = do_syscall(NR_write, fd, buffer, size);
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

int sys_clock_get(int clock, time_t *secs, long *nanos) STUB_ONLY

void sys_exit(int status) {
	do_syscall(NR_exit, status);
	__builtin_trap();
}

int sys_futex_wait(int *pointer, int expected) STUB_ONLY
int sys_futex_wake(int *pointer) STUB_ONLY

#endif // MLIBC_BUILDING_RTDL

} // namespace mlibc
