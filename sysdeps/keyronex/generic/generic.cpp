#include <asm/ioctls.h>

#include <keyronex/syscall.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include <bits/ensure.h>
#include <frg/logging.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {

/* libc */

void
sys_libc_log(const char *message)
{
	syscall1(SYS_debug_message, (uintptr_t)message, NULL);
}

void
sys_libc_panic()
{
	sys_libc_log("\nMLIBC PANIC\n");
	for (;;)
		;
}

int
sys_futex_tid()
{
	return syscall0(SYS_thread_gettid, NULL);
}

int
sys_futex_wait(int *pointer, int expected, const struct timespec *time)
{
	int r =syscall3(SYS_futex_wait, (uintptr_t)pointer, expected,
	    (uintptr_t)time, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_futex_wake(int *pointer)
{
	int r = syscall2(SYS_futex_wake, (uintptr_t)pointer, (uintptr_t)INT_MAX,
	    NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_tcb_set(void *pointer)
{
#if defined(__m68k__)
	syscall1(SYS_tcb_set, (uintptr_t)pointer + 0x7000 + sizeof(Tcb), NULL);
#elif defined(__amd64__)
	syscall1(SYS_tcb_set, (uintptr_t)pointer, NULL);
#elif defined(__aarch64__)
	uintptr_t addr = reinterpret_cast<uintptr_t>(pointer);
	addr += sizeof(Tcb) - 0x10;
	asm volatile("msr tpidr_el0, %0" ::"r"(addr));
#elif defined(__riscv)
	uintptr_t addr = reinterpret_cast<uintptr_t>(pointer);
	addr += sizeof(Tcb);
	asm volatile("mv tp, %0" ::"r"(addr));
#endif
	return 0;
}

void
*sys_tp_get()
{
	return (void*)syscall0(SYS_tcb_get, NULL);
}

/* vm */

int
sys_anon_allocate(size_t size, void **pointer)
{
	return sys_vm_map(NULL, size, PROT_READ | PROT_WRITE,
	    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0, pointer);
}

int
sys_anon_free(void *pointer, size_t size)
{
	return sys_vm_unmap(pointer, size);
}

int
sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset,
    void **window)
{
	uintptr_t addr;
	int r = syscall6(SYS_mmap, (uintptr_t)hint, size, prot,
	    flags, fd, offset, &addr);
	if (r < 0)
		return -r;
	*window = (void *)addr;
	return 0;
}

int
sys_vm_protect(void *pointer, size_t size, int prot)
{
	int r = syscall3(SYS_mprotect, (uintptr_t)pointer, size, prot, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_vm_unmap(void *pointer, size_t size)
{
	int r = syscall2(SYS_munmap, (uintptr_t)pointer, size, NULL);
	if (r < 0)
		return -r;
	return 0;
}

/* vfs */

int
sys_access(const char *path, int mode)
{
	return sys_faccessat(AT_FDCWD, path, mode, 0);
}

int
sys_faccessat(int dirfd, const char *pathname, int mode, int flags)
{
	int r = syscall4(SYS_faccessat, dirfd, (uintptr_t)pathname,
	    mode, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_open(const char *pathname, int flags, mode_t mode, int *fd)
{
	return sys_openat(AT_FDCWD, pathname, flags, mode, fd);
}

int
sys_open_dir(const char *path, int *handle)
{
    return sys_open(path, O_DIRECTORY, 0, handle);
}

int
sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd)
{
	int r = syscall4(SYS_openat, dirfd, (uintptr_t)path, flags, mode, NULL);
	if (r < 0)
		return -r;
	*fd = r;
	return 0;
}

int
sys_link(const char *old_path, const char *new_path)
{
	return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int
sys_linkat(int olddirfd, const char *old_path, int newdirfd,
    const char *new_path, int flags)
{
	int r = syscall5(SYS_linkat, olddirfd, (uintptr_t)old_path,
	    newdirfd, (uintptr_t)new_path, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_unlinkat(int fd, const char *path, int flags)
{
	int r = syscall3(SYS_unlinkat, fd, (uintptr_t)path, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_mkdirat(int dirfd, const char *path, mode_t mode)
{
	int r = syscall3(SYS_mkdirat, dirfd, (uintptr_t)path, mode, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_mkdir(const char *path, mode_t mode)
{
	return sys_mkdirat(AT_FDCWD, path, mode);
}

int sys_renameat(int olddirfd, const char *old_path, int newdirfd,
    const char *new_path)
{
	int r = syscall5(SYS_renameat, olddirfd, (uintptr_t)old_path,
	    newdirfd, (uintptr_t)new_path, 0, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int sys_rename(const char *path, const char *new_path)
{
	return sys_renameat(AT_FDCWD, path, AT_FDCWD, new_path);
}

int
sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags,
    struct stat *statbuf)
{
	uintptr_t r;

	switch (fsfdt) {
	case fsfd_target::path:
		fd = AT_FDCWD;
		break;

	case fsfd_target::fd:
		flags |= AT_EMPTY_PATH;

	case fsfd_target::fd_path:
		break;

	default:
		__ensure(!"stat: Invalid fsfdt");
		__builtin_unreachable();
	}

	r = syscall4(SYS_fstatat, fd, (uintptr_t)path, flags,
	    (uintptr_t)statbuf, NULL);
	return -r;
}

/* open file ops */

int
sys_close(int fd)
{
	int r = syscall1(SYS_close, fd, NULL);
	return -r;
}

int
sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read)
{
	int r = syscall3(SYS_read, fd, (uintptr_t)buf, count, NULL);
	if (r >= 0) {
		*bytes_read = r;
		return 0;
	}
	return -r;
}

int
sys_readv(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read)
{
	int r = syscall3(SYS_readv, fd, (uintptr_t)iovs, iovc, NULL);
	if (r < 0)
		return -r;
	*bytes_read = r;
	return 0;
}

int
sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read)
{
	int r = syscall3(SYS_getdents, handle, (uintptr_t)buffer,
	    max_size, NULL);
	if (r >= 0) {
		*bytes_read = r;
		return 0;
	}
	return -r;
}

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written)
{
	int r = syscall3(SYS_write, fd, (uintptr_t)buf, count, NULL);
	if (r >= 0) {
		*bytes_written = r;
		return 0;
	}
	return -r;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset)
{
	off_t r = syscall3(SYS_seek, fd, offset, whence, NULL);
	if (r >= 0) {
		*new_offset = r;
		return 0;
	}
	return -r;
}

int
sys_ioctl(int fd, unsigned long request, void *arg, int *result)
{
	int r = syscall3(SYS_ioctl, fd, request, (uintptr_t)arg,
	    NULL);
	if (r < 0)
		return -r;
	if (result != nullptr)
		*result = r;
	return 0;
}


int
sys_isatty(int fd)
{
	struct winsize ws;
	int result;

	if (sys_ioctl(fd, TIOCGWINSZ, &ws, &result) == 0)
		return 0;

	return ENOTTY;
}

int
sys_tcgetattr(int fd, struct termios *attr)
{
	return sys_ioctl(fd, TCGETS, (void *)attr, NULL);
}

int
sys_tcsetattr(int fd, int optional_action, const struct termios *attr)
{
	int req;

	switch (optional_action) {
		case TCSANOW: req = TCSETS; break;
		case TCSADRAIN: req = TCSETSW; break;
		case TCSAFLUSH: req = TCSETSF; break;
		default: return EINVAL;
	}

	return sys_ioctl(fd, req, (void *)attr, NULL);
}

#ifndef MLIBC_BUILDING_RTLD
int
sys_ptsname(int fd, char *buffer, size_t length)
{
	int index;
	if (int e = sys_ioctl(fd, TIOCGPTN, &index, NULL); e)
		return e;
	if ((size_t)snprintf(buffer, length, "/dev/pts%d", index) >= length) {
		return ERANGE;
	}
	return 0;
}
#endif

/* fd manipulation */

int
sys_dup(int fd, int flags, int *newfd) {
	int r = syscall2(SYS_dup, fd, flags, NULL);
	if (r < 0)
		return -r;
	*newfd = r;
	return 0;
}

int
sys_dup2(int fd, int flags, int newfd) {
	int r = syscall3(SYS_dup3, fd, newfd, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_fcntl(int fd, int request, va_list args, int *result)
{
	uintptr_t arg = va_arg(args, uintptr_t);
	int r = syscall3(SYS_fcntl, fd, request, arg, NULL);
	if (r < 0)
		return -r;
	if (result != nullptr)
		*result = r;
	return 0;
}

int
sys_pipe(int *fds, int flags)
{
	int r = syscall2(SYS_pipe, (uintptr_t)fds, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

/* time */

int
sys_sleep(time_t *secs, long *nanos)
{
	struct timespec rqtp, rmtp;
	int r;
	rqtp.tv_sec = *secs;
	rqtp.tv_nsec = *nanos;
	r = syscall4(SYS_clock_nanosleep, CLOCK_MONOTONIC, 0, (uintptr_t)&rqtp,
	    (uintptr_t)&rmtp, NULL);
	*secs = rmtp.tv_sec;
	*nanos = rmtp.tv_nsec;
	return r;
}

int
sys_clock_get(int clock, time_t *secs, long *nanos)
{
	struct timespec tp;
	int r = syscall2(SYS_clock_gettime, clock, (uintptr_t)&tp, NULL);
	if (r < 0)
		return -r;
	*secs = tp.tv_sec;
	*nanos = tp.tv_nsec;
	return 0;
}

int
sys_clock_getres(int clock, time_t *secs, long *nanos)
{
	struct timespec tp = {};
	int r = syscall2(SYS_clock_getres, clock, (uintptr_t)&tp, NULL);
	if (r < 0)
		return -r;
	*secs = tp.tv_sec;
	*nanos = tp.tv_nsec;
	return 0;
}

/* credentials */

int
sys_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid)
{
	uintptr_t r = syscall3(SYS_getresuid, (uintptr_t)ruid,
	    (uintptr_t)euid, (uintptr_t)suid, NULL);
	return -r;
}

int
sys_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid)
{
	uintptr_t r = syscall3(SYS_getresgid, (uintptr_t)rgid,
	    (uintptr_t)egid, (uintptr_t)sgid, NULL);
	return -r;
}

uid_t
sys_geteuid()
{
	uid_t r, e, s;
	__ensure(sys_getresuid(&r, &e, &s) == 0);
	return e;
}

uid_t
sys_getuid()
{
	uid_t r, e, s;
	__ensure(sys_getresuid(&r, &e, &s) == 0);
	return r;
}

gid_t
sys_getgid()
{
	gid_t r, e, s;
	__ensure(sys_getresgid(&r, &e, &s) == 0);
	return r;
}

gid_t
sys_getegid()
{
	gid_t r, e, s;
	__ensure(sys_getresgid(&r, &e, &s) == 0);
	return e;
}

/* misc */

int
sys_uname(struct utsname *buf) {
	int r = syscall1(SYS_utsname, (uintptr_t)buf, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
sys_gethostname(char *buf, size_t bufsize)
{
	struct utsname uname_buf;
	if (int r = sys_uname(&uname_buf); r != 0)
		return r;

	auto node_len = strlen(uname_buf.nodename);
	if (node_len >= bufsize)
		return ENAMETOOLONG;

	memcpy(buf, uname_buf.nodename, node_len);
	buf[node_len] = '\0';
	return 0;
}

} // namespace mlibc
