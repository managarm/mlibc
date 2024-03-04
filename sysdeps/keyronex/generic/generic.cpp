#include <asm/ioctls.h>

#include <bits/ensure.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <frg/logging.hpp>
#include <keyronex/syscall.h>
#include <limits.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <stdlib.h>

#define STUB_ONLY                                           \
	{                                                   \
		__ensure(!"STUB_ONLY function was called"); \
		__builtin_unreachable();                    \
	}

namespace mlibc {

void
sys_libc_log(const char *message)
{
	syscall1(kPXSysDebug, (uintptr_t)message, NULL);
}

void
sys_libc_panic()
{
	sys_libc_log("\nMLIBC PANIC\n");
	for (;;)
		;
	STUB_ONLY
}

void
sys_exit(int status)
{
	syscall1(kPXSysExit, status, NULL);
	mlibc::panicLogger() << "sys_exit() returned!" << frg::endlog;
	__builtin_unreachable();
}

#ifndef MLIBC_BUILDING_RTLD
int
sys_tcgetattr(int fd, struct termios *attr)
{
	int ret;

	if (int r = sys_ioctl(fd, TCGETS, attr, &ret) != 0) {
		return r;
	}

	return 0;
}

int
sys_tcsetattr(int fd, int optional_action, const struct termios *attr)
{
	int ret;

	switch (optional_action) {
	case TCSANOW:
		optional_action = TCSETS;
		break;
	case TCSADRAIN:
		optional_action = TCSETSW;
		break;
	case TCSAFLUSH:
		optional_action = TCSETSF;
		break;
	default:
		__ensure(!"Unsupported tcsetattr");
	}

	if (int r = sys_ioctl(fd, optional_action, (void *)attr, &ret) != 0) {
		return r;
	}

	return 0;
}
#endif

int
sys_tcb_set(void *pointer)
{
	return syscall1(kPXSysSetFSBase, (uintptr_t)pointer, NULL);
}

int
sys_ppoll(struct pollfd *fds, int nfds, const struct timespec *timeout,
    const sigset_t *sigmask, int *num_events)
{
	uintptr_t ret = syscall4(kPXSysPPoll, (uintptr_t)fds, (uintptr_t)nfds,
	    (uintptr_t)timeout, (uintptr_t)sigmask, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*num_events = (ssize_t)ret;
	return 0;
}

int
sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events)
{
	struct timespec ts;
	ts.tv_sec = timeout / 1000;
	ts.tv_nsec = (timeout % 1000) * 1000000;
	return sys_ppoll(fds, count, timeout < 0 ? NULL : &ts, NULL,
	    num_events);
}

#ifndef MLIBC_BUILDING_RTLD
int
sys_pselect(int nfds, fd_set *read_set, fd_set *write_set, fd_set *except_set,
    const struct timespec *timeout, const sigset_t *sigmask, int *num_events)
{
	struct pollfd *fds = (struct pollfd *)malloc(
	    nfds * sizeof(struct pollfd));

	for (int i = 0; i < nfds; i++) {
		struct pollfd *fd = &fds[i];
		memset(fd, 0, sizeof(struct pollfd));

		if (read_set && FD_ISSET(i, read_set))
			fd->events |= POLLIN; // TODO: Additional events.
		if (write_set && FD_ISSET(i, write_set))
			fd->events |= POLLOUT; // TODO: Additional events.
		if (except_set && FD_ISSET(i, except_set))
			fd->events |= POLLPRI;

		if (!fd->events) {
			fd->fd = -1;
			continue;
		}

		fd->fd = i;
	}

	int e = sys_ppoll(fds, nfds, timeout, sigmask, num_events);

	if (e != 0) {
		free(fds);
		return e;
	}

	fd_set res_read_set;
	fd_set res_write_set;
	fd_set res_except_set;
	FD_ZERO(&res_read_set);
	FD_ZERO(&res_write_set);
	FD_ZERO(&res_except_set);

	for (int i = 0; i < nfds; i++) {
		struct pollfd *fd = &fds[i];

		if (read_set && FD_ISSET(i, read_set) &&
		    fd->revents & (POLLIN | POLLERR | POLLHUP)) {
			FD_SET(i, &res_read_set);
		}

		if (write_set && FD_ISSET(i, write_set) &&
		    fd->revents & (POLLOUT | POLLERR | POLLHUP)) {
			FD_SET(i, &res_write_set);
		}

		if (except_set && FD_ISSET(i, except_set) &&
		    fd->revents & POLLPRI) {
			FD_SET(i, &res_except_set);
		}
	}

	free(fds);

	if (read_set)
		memcpy(read_set, &res_read_set, sizeof(fd_set));
	if (write_set)
		memcpy(write_set, &res_write_set, sizeof(fd_set));
	if (except_set)
		memcpy(except_set, &res_except_set, sizeof(fd_set));

	return 0;
}
#endif

int
sys_fcntl(int fd, int request, va_list args, int *result)
{
	auto ret = syscall3(kPXSysFCntl, fd, request, va_arg(args, uint64_t),
	    NULL);
	if (int e = sc_error(ret); e)
		return e;
	*result = ret;
	return 0;
}

int
sys_futex_wait(int *pointer, int expected, const struct timespec *time)
{
	auto ret = syscall3(kPXSysFutexWait, (uintptr_t)pointer, expected,
	    (uintptr_t)time, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_futex_wake(int *pointer)
{
	auto ret = syscall1(kPXSysFutexWake, (uintptr_t)pointer, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

#ifndef MLIBC_BUILDING_RTLD
int
sys_ioctl(int fd, unsigned long request, void *arg, int *result)
{
	uintptr_t r = syscall3(kPXSysIOCtl, fd, request, (uintptr_t)arg, NULL);
	if (int e = sc_error(r); e)
		return e;
	*result = r;
	return 0;
}
#endif

int
sys_isatty(int fd)
{
	uintptr_t ret = syscall1(kPXSysIsATTY, fd, NULL);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

#ifndef MLIBC_BUILDING_RTLD
int
sys_getcwd(char *buffer, size_t size)
{
	uintptr_t ret = syscall2(kPXSysGetCWD, (uintptr_t)buffer, size, NULL);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}
#endif

int
sys_dup(int fd, int flags, int *newfd)
{
	uintptr_t ret = syscall2(kPXSysDup, fd, flags, NULL);
	if (int e = sc_error(ret); e) {
		return e;
	}
	*newfd = ret;
	return 0;
}

int
sys_dup2(int fd, int flags, int newfd)
{
	uintptr_t ret = syscall3(kPXSysDup3, fd, newfd, flags, NULL);
	if (int e = sc_error(ret); e) {
		return e;
	}
	return 0;
}

int
sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd)
{
	uintptr_t r = syscall4(kPXSysOpenAt, dirfd, (uintptr_t)path,
	    (uintptr_t)flags, (uintptr_t)mode, NULL);
	if (int e = sc_error(r); e)
		return e;
	*fd = (int)r;
	return 0;
}

int
sys_open(const char *path, int flags, mode_t mode, int *fd)
{
	return sys_openat(AT_FDCWD, path, flags, mode, fd);
}

int
sys_open_dir(const char *path, int *handle)
{
	return sys_open(path, O_DIRECTORY, 0, handle);
}

int
sys_read_entries(int fd, void *buffer, size_t max_size, size_t *bytes_read)
{
	uintptr_t r = syscall3(kPXSysReadDir, fd, (uintptr_t)buffer, max_size,
	    NULL);

	if (int e = sc_error(r); e)
		return e;

	*bytes_read = r;
	return 0;
}

int
sys_close(int fd)
{
	return (int)syscall1(kPXSysClose, fd, NULL);
}

int
sys_link(const char *old_path, const char *new_path)
{
	uintptr_t ret = syscall2(kPXSysLink, (uintptr_t)old_path,
	    (uintptr_t)new_path, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_seek(int fd, off_t offset, int whence, off_t *new_offset)
{
	uintptr_t ret = syscall3(kPXSysSeek, fd, offset, whence, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*new_offset = (ssize_t)ret;
	return 0;
}

int
sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read)
{
	uintptr_t ret = syscall3(kPXSysRead, fd, (uintptr_t)buf,
	    (uintptr_t)count, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*bytes_read = (ssize_t)ret;
	return 0;
}

int
sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written)
{
	uintptr_t ret = syscall3(kPXSysWrite, fd, (uintptr_t)buf,
	    (uintptr_t)count, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*bytes_written = (ssize_t)ret;
	return 0;
}

int
sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length)
{
	uintptr_t ret = syscall3(kPXSysReadLink, (uintptr_t)path,
	    (uintptr_t)buffer, (uintptr_t)max_size, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*length = (ssize_t)ret;
	return 0;
}

int
sys_pipe(int *fds, int flags)
{
	uintptr_t ret = syscall2(kPXSysPipe, (uintptr_t)fds, flags, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_unlinkat(int fd, const char *path, int flags)
{
	uintptr_t ret = syscall3(kPXSysUnlinkAt, fd, (uintptr_t)path, flags,
	    NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset,
    void **window)
{
	uintptr_t r = syscall6(kPXSysMmap, (uintptr_t)hint, size, prot, flags,
	    fd, offset, NULL);
	if (int e = sc_error(r); e)
		return e;
	*window = (void *)r;
	return 0;
}

int
sys_vm_unmap(void *pointer, size_t size)
{
	uintptr_t r = syscall2(kPXSysMunmap, (uintptr_t)pointer, size, NULL);
	if (int e = sc_error(r); e)
		return e;
	return 0;
}

int
sys_vm_protect(void *pointer, size_t size, int prot)
{

	mlibc::infoLogger() << "mlibc: sys_vm_protect(" << pointer << ", "
			    << size << ", " << prot << "); stub!\n"
			    << frg::endlog;
	return 0;
}

int
sys_anon_allocate(size_t size, void **pointer)
{
	return sys_vm_map(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE,
	    MAP_ANONYMOUS, -1, 0, pointer);
}

int
sys_anon_free(void *pointer, size_t size)
{
	return sys_vm_unmap(pointer, size);
}

pid_t
sys_getpid()
{
	return syscall0(kPXSysGetPID, NULL);
}

pid_t
sys_getppid()
{
	return syscall0(kPXSysGetPPID, NULL);
}

uid_t
sys_getuid()
{
	return 0;
}

uid_t
sys_geteuid()
{
	return 0;
}

gid_t
sys_getgid()
{
	return 0;
}

int
sys_getsid(pid_t pid, pid_t *sid)
{
	auto ret = syscall1(kPXSysGetSID, pid, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*sid = (pid_t)(ret);
	return 0;
}

int
sys_setgid(gid_t gid)
{
	(void)gid;
	return 0;
}

int
sys_getpgid(pid_t pid, pid_t *out)
{
	auto ret = syscall1(kPXSysGetPGID, pid, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*out = (pid_t)(ret);
	return 0;
}

int
sys_setpgid(pid_t pid, pid_t pgid)
{
	auto ret = syscall2(kPXSysSetPGID, pid, pgid, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_setsid(pid_t *sid)
{
	auto ret = syscall0(kPXSysSetSID, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*sid = (pid_t)ret;
	return 0;
}

gid_t
sys_getegid()
{
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n"
			    << frg::endlog;
	return 0;
}

pid_t
sys_gettid()
{
	return syscall0(kPXSysGetTID, NULL);
}

int
sys_clock_get(int clock, time_t *secs, long *nanos)
{
	auto ret = syscall1(kPXSysClockGet, clock, NULL);
	*secs = ret / 1000000000;
	*nanos = ret % 1000000000;
	return 0;
}

int
sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags,
    struct stat *statbuf)
{
	uintptr_t r;
	enum posix_stat_kind kind;

	switch (fsfdt) {
	case fsfd_target::fd:
		kind = kPXStatKindFD;
		break;

	case fsfd_target::path:
		kind = kPXStatKindCWD;
		break;
	case fsfd_target::fd_path:
		kind = kPXStatKindAt;
		break;

	default:
		__ensure(!"stat: Invalid fsfdt");
		__builtin_unreachable();
	}

	r = syscall5(kPXSysStat, kind, fd, (uintptr_t)path, flags,
	    (uintptr_t)statbuf, NULL);
	if (int e = sc_error(r); e)
		return e;
	return 0;
}

int
sys_statfs(const char *path, struct statfs *buf)
{
	uintptr_t ret = syscall2(kPXSysStatFS, (uintptr_t)path, (uintptr_t)buf,
	    NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_statvfs(const char *path, struct statvfs *buf)
{
	struct statfs sb;
	uintptr_t ret = syscall2(kPXSysStatFS, (uintptr_t)path, (uintptr_t)&sb,
	    NULL);
	if (int e = sc_error(ret); e)
		return e;

	buf->f_bsize = sb.f_bsize;
	buf->f_frsize = sb.f_frsize;
	buf->f_blocks = sb.f_blocks;
	buf->f_bfree = sb.f_bfree;
	buf->f_bavail = sb.f_bavail;

	buf->f_files = sb.f_files;
	buf->f_ffree = sb.f_ffree;
	buf->f_favail = sb.f_ffree;

	buf->f_fsid = sb.f_fsid.__val[1] | (uint64_t)sb.f_fsid.__val[0] << 32;
	buf->f_flag = sb.f_flags;
	buf->f_namemax = sb.f_namelen;

	return 0;
}

int
sys_faccessat(int dirfd, const char *pathname, int mode, int flags)
{
	(void)flags;
	struct stat buf;
	if (int r = sys_stat(dirfd == AT_FDCWD ? fsfd_target::path :
						 fsfd_target::fd_path,
		dirfd, pathname, mode & AT_SYMLINK_FOLLOW, &buf)) {
		return r;
	}
	return 0;
}

int
sys_access(const char *path, int mode)
{
	return sys_faccessat(AT_FDCWD, path, mode, 0);
}

int
sys_fork(pid_t *child)
{
	uintptr_t ret = syscall0(kPXSysFork, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*child = (int)ret;
	return 0;
}

int
sys_execve(const char *path, char *const argv[], char *const envp[])
{
	uintptr_t ret = syscall3(kPXSysExecVE, (uintptr_t)path, (uintptr_t)argv,
	    (uintptr_t)envp, NULL);
	if (int e = sc_error(ret); e)
		return e;
	mlibc::panicLogger() << "execve returned! " << ret << frg::endlog;
	__builtin_unreachable();
}

int
sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru,
    pid_t *ret_pid)
{
	(void)ru;

	uintptr_t ret = syscall3(kPXSysWaitPID, pid, (uintptr_t)status, flags,
	    NULL);
	if (int e = sc_error(ret); e)
		return e;
	*ret_pid = (pid_t)ret;
	return 0;
}

#ifndef MLIBC_BUILDING_RTLD
int
sys_sleep(time_t *sec, long *nanosec)
{
	auto ret = syscall1(kPXSysSleep, *sec * 1000000000 + *nanosec, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_uname(struct utsname *buf)
{
	uintptr_t ret = syscall1(kPXSysUTSName, (uintptr_t)buf, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_gethostname(char *buf, size_t bufsize)
{
	struct utsname uname_buf;
	if (auto e = sys_uname(&uname_buf); e)
		return e;

	auto node_len = strlen(uname_buf.nodename);
	if (node_len >= bufsize)
		return ENAMETOOLONG;

	memcpy(buf, uname_buf.nodename, node_len);
	buf[node_len] = '\0';
	return 0;
}

int
sys_fsync(int)
{
	mlibc::infoLogger() << "mlibc: fsync is a stub" << frg::endlog;
	return 0;
}

int
sys_getentropy(void *buffer, size_t length)
{
	/* todo: improve lmao */
	mlibc::infoLogger() << "mlibc: getentropy is a stub" << frg::endlog;
	memset(buffer, 123, length);
	return 0;
}
#endif

int
sys_mkdir(const char *path, mode_t mode)
{
	return sys_mkdirat(AT_FDCWD, path, mode);
}

int
sys_mkdirat(int dirfd, const char *path, mode_t mode)
{
	uintptr_t ret = syscall3(kPXSysMkDirAt, dirfd, (uintptr_t)path, mode,
	    NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_chdir(const char *path)
{
	uintptr_t ret = syscall1(kPXSysChDir, (uintptr_t)path, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

int
sys_umask(mode_t mode, mode_t *old)
{
	uintptr_t ret = syscall1(kPXSysUMask, mode, NULL);
	if (int e = sc_error(ret); e)
		return e;
	*old = (mode_t)ret;
	return 0;
}

int
sys_rename(const char *old_path, const char *new_path)
{
	return sys_renameat(AT_FDCWD, old_path, AT_FDCWD, new_path);
}

int
sys_renameat(int old_dirfd, const char *old_path, int new_dirfd,
    const char *new_path)
{
	auto ret = syscall4(kPXSysRenameAt, old_dirfd, (uintptr_t)old_path,
	    new_dirfd, (uintptr_t)new_path, NULL);
	if (int e = sc_error(ret); e)
		return e;
	return 0;
}

} // namespace mlibc
