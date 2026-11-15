#include <bits/ensure.h>
#include <cryptix/syscall.h>
#include <errno.h>
#include <fcntl.h>
#include <mlibc/all-sysdeps.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

namespace mlibc {
int Sysdeps<Read>::operator()(int fd, void *buffer, size_t count, ssize_t *bytesRead) {
	auto ret = Syscall(SYS_READ, fd, buffer, count);
	if (auto e = syscall_error(ret); e)
		return e;

	*bytesRead = ssize_t(ret);
	return 0;
}
int
Sysdeps<Write>::operator()(int fd, void const *buffer, unsigned long bytes, long *bytesWritten) {
	auto ret = Syscall(SYS_WRITE, fd, buffer, bytes);
	if (auto e = syscall_error(ret); e)
		return e;

	*bytesWritten = ret;
	return 0;
}
int Sysdeps<Open>::operator()(const char *path, int flags, mode_t mode, int *fd) {
	auto ret = Syscall(SYS_OPENAT, AT_FDCWD, path, flags, mode);
	if (auto e = syscall_error(ret); e)
		return e;

	*fd = ret;
	return 0;
}
int Sysdeps<Close>::operator()(int fd) {
	auto ret = Syscall(SYS_CLOSE, fd);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

// Needed by ld.so as of mlibc 7.0, so it must not be hidden behind
// MLIBC_BUILDING_RTLD.
int Sysdeps<Stat>::operator()(
    fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf
) {
	uintptr_t ret;
	switch (fsfdt) {
		case fsfd_target::fd:
			ret = Syscall(SYS_FSTATAT, fd, "", flags | AT_EMPTY_PATH, statbuf);
			break;
		case fsfd_target::path:
			ret = Syscall(SYS_FSTATAT, AT_FDCWD, path, flags, statbuf);
			break;
		case fsfd_target::fd_path:
			ret = Syscall(SYS_FSTATAT, fd, path, flags, statbuf);
			break;

		default:
			__ensure(!"sys_stat: invalid fsfdt");
			__builtin_unreachable();
	}
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *newOffset) {
	auto ret = Syscall(SYS_LSEEK, fd, offset, whence);
	if (auto e = syscall_error(ret); e)
		return e;

	*newOffset = off_t(ret);
	return 0;
}
int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
	auto ret = Syscall(SYS_IOCTL, fd, request, arg);
	if (auto e = syscall_error(ret); e)
		return e;

	*result = ret;
	return 0;
}
int Sysdeps<Pread>::operator()(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	auto ret = Syscall(SYS_PREAD64, fd, buf, n, off);
	if (auto e = syscall_error(ret); e)
		return e;

	*bytes_read = ret;
	return 0;
}
int
Sysdeps<Pwrite>::operator()(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
	auto ret = Syscall(SYS_PWRITE64, fd, buf, n, off);
	if (auto e = syscall_error(ret); e)
		return e;

	*bytes_written = ret;
	return 0;
}

int Sysdeps<Access>::operator()(const char *path, int mode) {
	auto ret = Syscall(SYS_ACCESS, path, mode);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Pipe>::operator()(int *fds, int flags) {
	auto ret = Syscall(SYS_PIPE2, fds, flags);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
	// TODO(v1tr10l7): implement flags;
	(void)flags;
	auto ret = Syscall(SYS_DUP, fd);
	if (auto e = syscall_error(ret); e)
		return e;

	*newfd = ret;
	return 0;
}
int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
	// TODO(v1tr10l7): implement flags;
	(void)flags;
	auto ret = Syscall(SYS_DUP2, fd, newfd);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd) {
	auto ret = Syscall(SYS_SOCKET, family, type, protocol);
	if (auto e = syscall_error(ret); e)
		return e;

	*fd = ret;
	return 0;
}
int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	auto ret = Syscall(SYS_BIND, fd, addr_ptr, addr_length);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<Uname>::operator()(struct utsname *out) {
	auto ret = Syscall(SYS_UNAME, out);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result) {
	auto arg = va_arg(args, unsigned long);
	auto ret = Syscall(SYS_FCNTL, fd, request, arg);
	if (auto e = syscall_error(ret); e)
		return e;

	*result = ret;
	return 0;
}

int Sysdeps<GetCwd>::operator()(char *buffer, size_t size) {
	auto ret = Syscall(SYS_GETCWD, buffer, size);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Chdir>::operator()(const char *path) {
	auto ret = Syscall(SYS_CHDIR, path);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Fchdir>::operator()(int fd) {
	auto ret = Syscall(SYS_FCHDIR, fd);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Rename>::operator()(const char *old_path, const char *new_path) {
	auto ret = Syscall(SYS_RENAME, old_path, new_path);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
	auto ret = Syscall(SYS_MKDIR, path, mode);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Rmdir>::operator()(const char *path) {
	auto ret = Syscall(SYS_RMDIR, path);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
	auto ret = Syscall(SYS_LINK, old_path, new_path);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
	auto ret = Syscall(SYS_SYMLINK, target_path, link_path);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Readlink>::operator()(const char *path, void *buffer, size_t maxSize, ssize_t *length) {
	auto ret = Syscall(SYS_READLINK, path, buffer, maxSize);
	if (auto e = syscall_error(ret); e)
		return e;

	*length = ret;
	return 0;
}
int Sysdeps<Chmod>::operator()(const char *pathname, mode_t mode) {
	auto ret = Syscall(SYS_CHMOD, pathname, mode);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Fchmod>::operator()(int fd, mode_t mode) {
	auto ret = Syscall(SYS_FCHMOD, fd, mode);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Umask>::operator()(mode_t mode, mode_t *old_mode) {
	auto ret = Syscall(SYS_UMASK, mode);
	if (auto e = syscall_error(ret); e)
		return e;

	*old_mode = ret;
	return 0;
}

int Sysdeps<Statfs>::operator()(const char *path, struct statfs *out) {
	auto ret = Syscall(SYS_STATFS, path, out);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
void Sysdeps<Sync>::operator()() { Syscall(SYS_SYNC); }
int Sysdeps<Mount>::operator()(
    const char *source,
    const char *target,
    const char *fs_name,
    unsigned long flags,
    const void *data
) {
	auto ret = Syscall(SYS_MOUNT, source, target, fs_name, flags, data);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<ReadEntries>::operator()(int fdnum, void *buffer, size_t max_size, size_t *bytes_read) {
	auto ret = Syscall(SYS_GETDENTS64, fdnum, buffer, max_size);
	if (auto e = syscall_error(ret); e)
		return e;

	*bytes_read = ret;
	return 0;
}
int Sysdeps<Openat>::operator()(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	auto ret = Syscall(SYS_OPENAT, dirfd, path, flags, mode);
	if (auto e = syscall_error(ret); e)
		return e;

	*fd = ret;
	return 0;
}
int Sysdeps<OpenDir>::operator()(const char *path, int *fd) {
	return sysdep<Openat>(AT_FDCWD, path, O_DIRECTORY, 0, fd);
}
int Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode) {
	auto ret = Syscall(SYS_MKDIRAT, dirfd, path, mode);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Mknodat>::operator()(int dirfd, const char *path, int mode, int dev) {
	auto ret = Syscall(SYS_MKNODAT, dirfd, path, mode, dev);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Unlinkat>::operator()(int dirfd, const char *path, int flags) {
	auto ret = Syscall(SYS_UNLINKAT, dirfd, path, flags);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Renameat>::operator()(
    int olddirfd, const char *old_path, int newdirfd, const char *new_path
) {
	auto ret = Syscall(SYS_RENAMEAT, olddirfd, old_path, newdirfd, new_path);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Linkat>::operator()(
    int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags
) {
	auto ret = Syscall(SYS_LINKAT, olddirfd, old_path, newdirfd, new_path, flags);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
	auto ret = Syscall(SYS_SYMLINKAT, target_path, dirfd, link_path);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Readlinkat>::operator()(
    int dirfd, const char *path, void *buf, size_t bufsz, ssize_t *length
) {
	auto ret = Syscall(SYS_READLINKAT, dirfd, path, buf, bufsz);
	if (auto e = syscall_error(ret); e)
		return e;

	*length = ret;
	return 0;
}
int Sysdeps<Fchmodat>::operator()(int fd, const char *pathname, mode_t mode, int flags) {
	auto ret = Syscall(SYS_FCHMODAT, fd, pathname, mode, flags);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<Isatty>::operator()(int fd) {
	winsize ws;
	int result;
	if (!sysdep<Ioctl>(fd, TIOCGWINSZ, &ws, &result))
		return 0;
	return ENOTTY;
}
#ifndef MLIBC_BUILDING_RTLD
int Sysdeps<Ttyname>::operator()(int fd, char *buf, size_t size) {
	if (int e = sysdep<Isatty>(fd); e)
		return e;

	pid_t self = getpid();
	char *path = nullptr;

	asprintf(&path, "/proc/%i/fd/%i", self, fd);
	__ensure(path);

	ssize_t l = readlink(path, buf, size);
	free(path);

	if (l < 0)
		return errno;
	else if ((size_t)l >= size)
		return ERANGE;

	buf[l] = '\0';
	struct stat st1;
	struct stat st2;

	if (stat(buf, &st1) || fstat(fd, &st2))
		return errno;
	if (st1.st_dev != st2.st_dev || st1.st_ino != st2.st_ino)
		return ENODEV;

	return 0;
}
#endif

int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr) {
	auto ret = Syscall(SYS_IOCTL, fd, TCGETS, attr);
	if (int e = syscall_error(ret); e)
		return e;
	return 0;
}
int Sysdeps<Tcsetattr>::operator()(int fd, int optional_action, const struct termios *attr) {
	int req;

	switch (optional_action) {
		case TCSANOW:
			req = TCSETS;
			break;
		case TCSADRAIN:
			req = TCSETSW;
			break;
		case TCSAFLUSH:
			req = TCSETSF;
			break;
		default:
			return EINVAL;
	}

	auto ret = Syscall(SYS_IOCTL, fd, req, attr);
	if (int e = syscall_error(ret); e)
		return e;
	return 0;
}
int Sysdeps<Tcflush>::operator()(int fd, int queue) {
	auto ret = Syscall(SYS_IOCTL, fd, TCFLSH, queue);
	if (int e = syscall_error(ret); e)
		return e;
	return 0;
}
int Sysdeps<Tcdrain>::operator()(int fd) {
	auto ret = Syscall(SYS_IOCTL, fd, TCSBRK, 1);
	if (int e = syscall_error(ret); e)
		return e;
	return 0;
}
int Sysdeps<Tcflow>::operator()(int fd, int action) {
	auto ret = Syscall(SYS_IOCTL, fd, TCXONC, action);
	if (int e = syscall_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Pselect>::operator()(
    int nfds,
    fd_set *readfds,
    fd_set *writefds,
    fd_set *exceptfds,
    const struct timespec *timeout,
    const sigset_t *sigmask,
    int *num_events
) {
	auto ret = Syscall(SYS_PSELECT6, nfds, readfds, writefds, exceptfds, timeout, sigmask);
	if (auto e = syscall_error(ret); e)
		return e;

	*num_events = ret;
	return 0;
}
int Sysdeps<Utimensat>::operator()(
    int dirfd, const char *pathname, const struct timespec times[2], int flags
) {
	auto ret = Syscall(SYS_UTIMENSAT, dirfd, pathname, times, flags);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Syncfs>::operator()(int fd) {
	auto ret = Syscall(SYS_SYNCFS, fd);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<Unlockpt>::operator()(int fd) {
	int unlock = 0;
	int result = 0;

	return sysdep<Ioctl>(fd, TIOCSPTLCK, &unlock, &result);
}
#ifndef MLIBC_BUILDING_RTLD
int Sysdeps<Ptsname>::operator()(int fd, char *buffer, size_t length) {
	unsigned int index;
	int result;
	if (int e = sysdep<Ioctl>(fd, TIOCGPTN, &index, &result); e)
		return e;
	if (static_cast<size_t>(snprintf(buffer, length, "/dev/pts/%u", index)) >= length)
		return ERANGE;

	return 0;
}
#endif
} // namespace mlibc
