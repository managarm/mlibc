#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/arch-defs.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <stdlib.h>

#include <wivos/syscall.h>

#define STUB_ONLY {                             \
	__ensure(!"STUB_ONLY function was called"); \
	__builtin_unreachable();                    \
}

namespace mlibc {
	void sys_libc_log(const char *message)  {
		syscall1(SYS_DEBUG, (uint64_t)message);
	}

	[[noreturn]] void sys_libc_panic() {
		sys_libc_log("mlibc Panic");
		while(1);
	}

	int sys_tcb_set(void *pointer) {
		return syscall1(SYS_TCB_SET, (uint64_t)pointer);
	}

	int sys_futex_wait(int *pointer, int expected, const struct timespec *time) STUB_ONLY
	int sys_futex_wake(int *pointer) STUB_ONLY

	int sys_anon_allocate(size_t size, void **pointer) {
		*pointer = (void *)syscall2(SYS_ALLOC, (uint64_t)0, size / page_size);

		return 0;
	}

	int sys_anon_free(void *pointer, size_t size) { //Stub, for now we don't free
		(void)pointer; (void)size;
		return 0;
	}

	int sys_open(const char *pathname, int flags, mode_t mode, int *fd) {
		int ret = syscall2(SYS_OPEN, (uint64_t)pathname, flags);
		if(ret < 0) return -1; //TODO: Errno

		*fd = ret;
		return 0;
	}
	int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
		ssize_t retValue = syscall3(SYS_READ, fd, (uint64_t)buf, count);
		if(retValue < 0) {
			if(bytes_read) *bytes_read = 0;
			return -1; //TODO: Errno
		}

		if(bytes_read) *bytes_read = (size_t)retValue;
		return 0;
	}
	int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
		size_t retValue = syscall3(SYS_SEEK, fd, (uint64_t)offset, whence);
		if(retValue < 0) return -1; //TODO: Errno

		*new_offset = retValue;
		return 0;
	}
	int sys_close(int fd) {
		int ret = syscall1(SYS_CLOSE, (uint64_t)fd);
		if(ret < 0) return -1; //TODO: Errno

		return 0;
	}

	// mlibc assumes that anonymous memory returned by sys_vm_map() is zeroed by the kernel / whatever is behind the sysdeps
	int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
		__ensure(flags & MAP_ANONYMOUS);

		size_t pageCount = (size + page_size - 1) / page_size;
		size_t resValue = syscall2(SYS_ALLOC, (uint64_t)hint, pageCount);
		if(!resValue) return -1;

		*window = (void *)resValue;
		return 0;
	}
	int sys_vm_unmap(void *pointer, size_t size) STUB_ONLY

	[[noreturn]] void sys_exit(int status) STUB_ONLY

	int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written)  {
		size_t retValue = syscall3(SYS_WRITE, fd, (uint64_t)buf, count);
		if(retValue < 0) {
			if(bytes_written) *bytes_written = 0;
			return -1; //TODO: Errno
		}

		if(bytes_written) *bytes_written = retValue;
		return 0;
	}

	int sys_clock_get(int clock, time_t *secs, long *nanos) STUB_ONLY

	int sys_fork(pid_t *child) {
		pid_t pid = syscall0(SYS_FORK);
		if(pid < 0) {
			//errno = pid;
			return -1;
		}

		*child = pid;
		return 0;
	}

	int sys_ioctl(int fd, uint64_t request, void *arg, int *result) {
		sc_qword_t ret = syscall3(SYS_IOCTL, fd, request, (sc_qword_t)arg);

		if ((int64_t)ret < 0)
			return ret;

		*result = (int32_t)(int64_t)ret;
		return 0;
	}

	int sys_isatty(int fd) {
		struct winsize ws;
		int ret;

		if (!sys_ioctl(fd, TIOCGWINSZ, &ws, &ret))
			return 0;

		return ENOTTY;
	}

	int sys_execve(const char *path, char *const argv[], char *const envp[]){
		sc_qword_t ret = syscall3(SYS_EXECVE, (sc_qword_t)path, (sc_qword_t)argv, (sc_qword_t)envp);

		return (int)ret;
	}
};