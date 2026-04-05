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
#include <mlibc/tcb.hpp>

namespace mlibc {

/* libc */

void
Sysdeps<LibcLog>::operator()(const char *message)
{
	syscall1(SYS_debug_message, (uintptr_t)message, NULL);
}

void
Sysdeps<LibcPanic>::operator()()
{
	sysdep<LibcLog>("\nMLIBC PANIC\n");
	for (;;)
		;
}

int
Sysdeps<FutexTid>::operator()()
{
	return syscall0(SYS_thread_gettid, NULL);
}

int
Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time)
{
	int r =syscall3(SYS_futex_wait, (uintptr_t)pointer, expected,
	    (uintptr_t)time, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<FutexWake>::operator()(int *pointer, bool all)
{
	int r = syscall2(SYS_futex_wake, (uintptr_t)pointer,
	    (uintptr_t)(all ? INT_MAX : 1), NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<TcbSet>::operator()(void *pointer)
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

#if defined (__m68k__)
extern "C" void *
__m68k_read_tp (void)
{
	return (void*)syscall0(SYS_tcb_get, NULL);
}
#endif

/* vm */

int
Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer)
{
	return sysdep<VmMap>(nullptr, size, PROT_READ | PROT_WRITE,
	    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0, pointer);
}

int
Sysdeps<AnonFree>::operator()(void *pointer, size_t size)
{
	return sysdep<VmUnmap>(pointer, size);
}

int
Sysdeps<VmMap>::operator()(void *hint, size_t size, int prot, int flags, int fd, off_t offset,
    void **window)
{
	uintptr_t r = syscall6(SYS_mmap, (uintptr_t)hint, size, prot,
	    flags, fd, offset, NULL);
	if (r > -4096UL)
		return -r;
	*window = (void *)r;
	return 0;
}

int
Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot)
{
	int r = syscall3(SYS_mprotect, (uintptr_t)pointer, size, prot, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<VmUnmap>::operator()(void *pointer, size_t size)
{
	int r = syscall2(SYS_munmap, (uintptr_t)pointer, size, NULL);
	if (r < 0)
		return -r;
	return 0;
}

/* vfs */

int
Sysdeps<Access>::operator()(const char *path, int mode)
{
	return sysdep<Faccessat>(AT_FDCWD, path, mode, 0);
}

int
Sysdeps<Faccessat>::operator()(int dirfd, const char *pathname, int mode, int flags)
{
	int r = syscall4(SYS_faccessat, dirfd, (uintptr_t)pathname,
	    mode, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd)
{
	return sysdep<Openat>(AT_FDCWD, pathname, flags, mode, fd);
}

int
Sysdeps<OpenDir>::operator()(const char *path, int *handle)
{
    return sysdep<Open>(path, O_DIRECTORY, 0, handle);
}

int
Sysdeps<Openat>::operator()(int dirfd, const char *path, int flags, mode_t mode, int *fd)
{
	int r = syscall4(SYS_openat, dirfd, (uintptr_t)path, flags, mode, NULL);
	if (r < 0)
		return -r;
	*fd = r;
	return 0;
}

int
Sysdeps<Link>::operator()(const char *old_path, const char *new_path)
{
	return sysdep<Linkat>(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int
Sysdeps<Linkat>::operator()(int olddirfd, const char *old_path, int newdirfd,
    const char *new_path, int flags)
{
	int r = syscall5(SYS_linkat, olddirfd, (uintptr_t)old_path,
	    newdirfd, (uintptr_t)new_path, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Unlinkat>::operator()(int fd, const char *path, int flags)
{
	int r = syscall3(SYS_unlinkat, fd, (uintptr_t)path, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode)
{
	int r = syscall3(SYS_mkdirat, dirfd, (uintptr_t)path, mode, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Mkdir>::operator()(const char *path, mode_t mode)
{
	return sysdep<Mkdirat>(AT_FDCWD, path, mode);
}

int
Sysdeps<Renameat>::operator()(int olddirfd, const char *old_path, int newdirfd,
    const char *new_path)
{
	int r = syscall5(SYS_renameat, olddirfd, (uintptr_t)old_path,
	    newdirfd, (uintptr_t)new_path, 0, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Rename>::operator()(const char *path, const char *new_path)
{
	return sysdep<Renameat>(AT_FDCWD, path, AT_FDCWD, new_path);
}

int
Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char *path, int flags,
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

int
Sysdeps<Readlinkat>::operator()(int dirfd, const char *path, void *buffer,
    size_t max_size, ssize_t *length)
{
	int r = syscall4(SYS_readlinkat, dirfd, (uintptr_t)path,
	    (uintptr_t)buffer, max_size, NULL);
	if (r < 0)
		return -r;
	*length = r;
	return 0;
}

int
Sysdeps<Readlink>::operator()(const char *path, void *buffer, size_t max_size,
    ssize_t *length)
{
	return sysdep<Readlinkat>(AT_FDCWD, path, buffer, max_size, length);
}

int
Sysdeps<Truncate>::operator()(const char *path, off_t length)
{
	int r = syscall2(SYS_truncate, (uintptr_t)path, length, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Fchmodat>::operator()(int dirfd, const char *pathname, mode_t mode,
    int flags)
{
	int r = syscall4(SYS_fchmodat, dirfd, (uintptr_t)pathname, mode, flags,
	    NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Fchmod>::operator()(int fd, mode_t mode)
{
	return sysdep<Fchmodat>(fd, "", mode, AT_EMPTY_PATH);
}

int
Sysdeps<Chmod>::operator()(const char *pathname, mode_t mode)
{
	return sysdep<Fchmodat>(AT_FDCWD, pathname, mode, 0);
}

int
Sysdeps<Fchownat>::operator()(int dirfd, const char *pathname, uid_t owner,
    gid_t group, int flags)
{
	int r = syscall5(SYS_fchownat, dirfd, (uintptr_t)pathname, owner,
	    group, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

/* open file ops */

int
Sysdeps<Close>::operator()(int fd)
{
	int r = syscall1(SYS_close, fd, NULL);
	return -r;
}

int
Sysdeps<Read>::operator()(int fd, void *buf, size_t count, ssize_t *bytes_read)
{
	int r = syscall3(SYS_read, fd, (uintptr_t)buf, count, NULL);
	if (r >= 0) {
		*bytes_read = r;
		return 0;
	}
	return -r;
}

int
Sysdeps<Readv>::operator()(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read)
{
	int r = syscall3(SYS_readv, fd, (uintptr_t)iovs, iovc, NULL);
	if (r < 0)
		return -r;
	*bytes_read = r;
	return 0;
}

int
Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read)
{
	int r = syscall3(SYS_getdents, handle, (uintptr_t)buffer,
	    max_size, NULL);
	if (r >= 0) {
		*bytes_read = r;
		return 0;
	}
	return -r;
}

int
Sysdeps<Write>::operator()(int fd, const void *buf, size_t count, ssize_t *bytes_written)
{
	int r = syscall3(SYS_write, fd, (uintptr_t)buf, count, NULL);
	if (r >= 0) {
		*bytes_written = r;
		return 0;
	}
	return -r;
}

int
Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset)
{
	off_t r = syscall3(SYS_seek, fd, offset, whence, NULL);
	if (r >= 0) {
		*new_offset = r;
		return 0;
	}
	return -r;
}

int
Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result)
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
Sysdeps<Ftruncate>::operator()(int fd, size_t size)
{
	int r = syscall2(SYS_ftruncate, fd, size, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Flock>::operator()(int fd, int options)
{
	int r = syscall2(SYS_flock, fd, options, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Isatty>::operator()(int fd)
{
	struct winsize ws;
	int result;

	if (sysdep<Ioctl>(fd, TIOCGWINSZ, &ws, &result) == 0)
		return 0;

	return ENOTTY;
}

int
Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr)
{
	return sysdep<Ioctl>(fd, TCGETS, (void *)attr, nullptr);
}

int
Sysdeps<Tcsetattr>::operator()(int fd, int optional_action, const struct termios *attr)
{
	int req;

	switch (optional_action) {
		case TCSANOW: req = TCSETS; break;
		case TCSADRAIN: req = TCSETSW; break;
		case TCSAFLUSH: req = TCSETSF; break;
		default: return EINVAL;
	}

	return sysdep<Ioctl>(fd, req, (void *)attr, nullptr);
}

#ifndef MLIBC_BUILDING_RTLD
int
Sysdeps<Ptsname>::operator()(int fd, char *buffer, size_t length)
{
	int index;
	if (int e = sysdep<Ioctl>(fd, TIOCGPTN, &index, nullptr); e)
		return e;
	if ((size_t)snprintf(buffer, length, "/dev/pts%d", index) >= length) {
		return ERANGE;
	}
	return 0;
}
#endif

/* fd manipulation */

int
Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
	int r = syscall2(SYS_dup, fd, flags, NULL);
	if (r < 0)
		return -r;
	*newfd = r;
	return 0;
}

int
Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
	int r = syscall3(SYS_dup3, fd, newfd, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result)
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
Sysdeps<Pipe>::operator()(int *fds, int flags)
{
	int r = syscall2(SYS_pipe, (uintptr_t)fds, flags, NULL);
	if (r < 0)
		return -r;
	return 0;
}

/* time */

int
Sysdeps<Sleep>::operator()(time_t *secs, long *nanos)
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
Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos)
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
Sysdeps<ClockGetres>::operator()(int clock, time_t *secs, long *nanos)
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
Sysdeps<GetResuid>::operator()(uid_t *ruid, uid_t *euid, uid_t *suid)
{
	uintptr_t r = syscall3(SYS_getresuid, (uintptr_t)ruid,
	    (uintptr_t)euid, (uintptr_t)suid, NULL);
	return -r;
}

int
Sysdeps<GetResgid>::operator()(gid_t *rgid, gid_t *egid, gid_t *sgid)
{
	uintptr_t r = syscall3(SYS_getresgid, (uintptr_t)rgid,
	    (uintptr_t)egid, (uintptr_t)sgid, NULL);
	return -r;
}

uid_t
Sysdeps<GetEuid>::operator()()
{
	uid_t r, e, s;
	__ensure(sysdep<GetResuid>(&r, &e, &s) == 0);
	return e;
}

uid_t
Sysdeps<GetUid>::operator()()
{
	uid_t r, e, s;
	__ensure(sysdep<GetResuid>(&r, &e, &s) == 0);
	return r;
}

gid_t
Sysdeps<GetGid>::operator()()
{
	gid_t r, e, s;
	__ensure(sysdep<GetResgid>(&r, &e, &s) == 0);
	return r;
}

gid_t
Sysdeps<GetEgid>::operator()()
{
	gid_t r, e, s;
	__ensure(sysdep<GetResgid>(&r, &e, &s) == 0);
	return e;
}

/* misc */

int
Sysdeps<Uname>::operator()(struct utsname *buf) {
	int r = syscall1(SYS_utsname, (uintptr_t)buf, NULL);
	if (r < 0)
		return -r;
	return 0;
}

int
Sysdeps<GetHostname>::operator()(char *buf, size_t bufsize)
{
	struct utsname uname_buf;
	if (int r = sysdep<Uname>(&uname_buf); r != 0)
		return r;

	auto node_len = strlen(uname_buf.nodename);
	if (node_len >= bufsize)
		return ENAMETOOLONG;

	memcpy(buf, uname_buf.nodename, node_len);
	buf[node_len] = '\0';
	return 0;
}

} // namespace mlibc
