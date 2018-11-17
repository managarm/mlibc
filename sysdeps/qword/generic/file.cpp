
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

namespace mlibc {

void sys_libc_log(const char *message) {
	unsigned long res;
	asm volatile ("syscall" : "=a"(res)
			: "a"(0), "D"(0), "S"(message)
			: "rcx", "r11");
}

void sys_libc_panic() {
	__builtin_trap();
}

int sys_tcb_set(void *pointer) {
	return -1;
}

int sys_anon_allocate(size_t size, void **pointer) {
	// The qword kernel wants us to allocate whole pages.
	__ensure(!(size & 0xFFF));

	void *res;
	asm volatile ("syscall" : "=a"(res)
			: "a"(6), "D"(0), "S"(size >> 12)
			: "rcx", "r11");
	if(!res)
		return -1;
	*pointer = res;
	return 0;
}

int sys_anon_free(void *pointer, size_t size) {
	return -1;
}

#ifndef MLIBC_BUILDING_RTDL
void sys_exit(int status) {
	__builtin_trap();
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_clock_get(int clock, time_t *secs, long *nanos) {
	return -1;
}
#endif

int sys_open(const char *path, int flags, int *fd) {
	return -1;
}

int sys_close(int fd) {
	return -1;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	return -1;
}

#ifndef MLIBC_BUILDING_RTDL
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	return -1;
}
#endif

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	return -1;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	return -1;
}

#ifndef MLIBC_BUILDING_RTDL
int sys_fstat(int fd, struct stat *statbuf) {
	return -1;
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_rename(const char *path, const char *new_path) {
	return -1;
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict) {
	return -1;
}
#endif

} // namespace mlibc

