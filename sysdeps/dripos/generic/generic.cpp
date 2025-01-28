#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread-entry.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>

namespace mlibc {

void sys_libc_log(const char *message) {
	unsigned long res;
	asm volatile ("syscall" : "=a"(res)
			: "a"(50), "D"(message)
			: "rcx", "r11", "rdx");
}

void sys_libc_panic() {
	mlibc::infoLogger() << "\e[31mmlibc: panic!" << frg::endlog;
	asm volatile ("syscall" :
			: "a"(12), "D"(1)
			: "rcx", "r11", "rdx");
}

int sys_tcb_set(void *pointer) {
	int res;
	asm volatile ("syscall" : "=a"(res)
			: "a"(300), "D"(pointer)
			: "rcx", "r11", "rdx");
	return res;
}

int sys_anon_allocate(size_t size, void **pointer) {
	void *ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(9), "D"(0), "S"(size)
			: "rcx", "r11");

	if (!ret)
		return sys_errno;

	*pointer = ret;
	return 0;
}

int sys_anon_free(void *pointer, size_t size) {
	int unused_return;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(unused_return), "=d"(sys_errno)
			: "a"(11), "D"(pointer), "S"(size)
			: "rcx", "r11");

	if (unused_return)
		return sys_errno;

	return 0;
}

#ifndef MLIBC_BUILDING_RTLD
void sys_exit(int status) {
	asm volatile ("syscall" :
			: "a"(12), "D"(status)
			: "rcx", "r11", "rdx");
}
#endif

#ifndef MLIBC_BUILDING_RTLD
int sys_clock_get(int clock, time_t *secs, long *nanos) {
	return 0;
}
#endif

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	int ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(2), "D"(path), "S"(flags), "d"(0)
			: "rcx", "r11");

	if (ret == -1)
		return sys_errno;

	*fd = ret;
	return 0;
}

int sys_close(int fd) {
	int ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(3), "D"(fd)
			: "rcx", "r11");

	if (ret == -1)
		return sys_errno;

	return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	ssize_t ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(0), "D"(fd), "S"(buf), "d"(count)
			: "rcx", "r11");

	if (ret == -1)
		return sys_errno;

	*bytes_read = ret;
	return 0;
}

#ifndef MLIBC_BUILDING_RTLD
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	ssize_t ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(1), "D"(fd), "S"(buf), "d"(count)
			: "rcx", "r11");

	if (ret == -1)
		return sys_errno;

	*bytes_written = ret;
	return 0;
}
#endif


int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	off_t ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(8), "D"(fd), "S"(offset), "d"(whence)
			: "rcx", "r11");

	if (ret == -1)
		return sys_errno;

	*new_offset = ret;
	return 0;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags,
		int fd, off_t offset, void **window) {
	__ensure(flags & MAP_ANONYMOUS);
	void *ret;
	int sys_errno;

	// mlibc::infoLogger() << "calling sys_vm_map with size: " << size << frg::endlog;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(9), "D"(hint), "S"(size)
			: "rcx", "r11");

	if (!ret)
		return sys_errno;

	*window = ret;

	return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
	return sys_anon_free(pointer, size);
}

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	uint64_t err;
	asm volatile ("syscall"
			: "=d"(err)
			: "a"(66), "D"(pointer), "S"(expected)
			: "rcx", "r11");

	if (err) {
		return -1;
	}

	return 0;
}

int sys_futex_wake(int *pointer) {
	uint64_t err;
	asm volatile ("syscall"
			: "=d"(err)
			: "a"(65), "D"(pointer)
			: "rcx", "r11");

	if (err) {
		return -1;
	}

	return 0;
}

// All remaining functions are disabled in ldso.
#ifndef MLIBC_BUILDING_RTLD

int sys_clone(void *tcb, pid_t *tid_out, void *stack) {
	int tid;

	asm volatile ("syscall"
		: "=a"(tid)
		: "a"(67), "D"(__mlibc_start_thread), "S"(stack), "d"(tcb)
		: "rcx", "r11");

	if (tid_out)
		*tid_out = tid;

	return 0;
}

void sys_thread_exit() {
	asm volatile ("syscall"
			:
			: "a"(68)
			: "rcx", "r11");
	__builtin_trap();
}

int sys_sleep(time_t *secs, long *nanos) {
	long ms = (*nanos / 1000000) + (*secs * 1000);
	asm volatile ("syscall"
		:
		: "a"(6), "D"(ms)
		: "rcx", "r11");
	*secs = 0;
	*nanos = 0;
	return 0;
}

int sys_fork(pid_t *child) {
	pid_t ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(57)
			: "rcx", "r11");

	if (ret == -1)
		return sys_errno;

	*child = ret;
	return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	int ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(59), "D"(path), "S"(argv), "d"(envp)
			: "rcx", "r11");

	if (sys_errno != 0)
		return sys_errno;

	return 0;
}

pid_t sys_getpid() {
	pid_t pid;
	asm volatile ("syscall" : "=a"(pid)
			: "a"(5)
			: "rcx", "r11", "rdx");
	return pid;
}
pid_t sys_getppid() {
	pid_t ppid;
	asm volatile ("syscall" : "=a"(ppid)
			: "a"(14)
			: "rcx", "r11", "rdx");
	return ppid;
}

#endif // MLIBC_BUILDING_RTLD

} // namespace mlibc
