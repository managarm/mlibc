#include "include/abi-bits/fcntl.h"
#include "mlibc/tcb.hpp"
#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <bits/syscall.h>
#include <cstddef>
#include <cstdint>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>

// ANCHOR: stub
#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})
// ANCHOR_END: stub

namespace mlibc {

void sys_libc_panic() {
	sys_libc_log("!!! mlibc panic !!!");
	sys_exit(-1);
	__builtin_trap();
}

void sys_libc_log(const char *msg) { __syscall_temp_write(msg, strlen(msg)); }

int sys_isatty(int fd) {
	(void)fd;
	return 0;
}

int sys_write(int fd, void const *buf, size_t size, ssize_t *ret) {
	*ret = __syscall_fs_write(fd, buf, size);
	return *ret >= 0 ? 0 : -*ret;
}

int sys_tcb_set(void *pointer) {
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile("mv tp, %0" ::"r"(thread_data));
	return 0;
}

int sys_anon_allocate(size_t size, void **pointer) {
	auto res =
	    __syscall_mem_map(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS);
	*pointer = (void *)res;
	ssize_t errno = reinterpret_cast<intptr_t>(res);
	return errno < 0 ? -errno : 0;
}

int sys_anon_free(void *pointer, unsigned long size) {
	__syscall_mem_unmap(pointer, size);
	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *ret) {
	auto res = __syscall_fs_seek(fd, offset, whence);
	*ret = res;
	return res < 0 ? -res : 0;
}

void sys_exit(int status) {
	__syscall_proc_exit(status);
	__builtin_unreachable();
}

int sys_close(int fd) { return -__syscall_fs_close(fd); }

int sys_futex_wake(int *) { STUB(); }

int sys_futex_wait(int *, int, timespec const *) { STUB(); }

int sys_read(int fd, void *read_buf, unsigned long read_len, long *ret) {
	auto res = __syscall_fs_read(fd, read_buf, read_len);
	*ret = res;
	return res < 0 ? -res : 0;
}

int sys_open(const char *path, int oflags, unsigned int mode, int *ret) {
	(void)mode;
	// TODO: Mode currently ignored.
	auto res = __syscall_fs_open(AT_FDCWD, path, oflags);
	*ret = res;
	return res < 0 ? -res : 0;
}

int sys_vm_map(void *, size_t, int, int, int, off_t, void **) { STUB(); }

int sys_vm_unmap(void *, size_t) { STUB(); }

int sys_clock_get(int, time_t *, long *) { STUB(); }

} // namespace mlibc
