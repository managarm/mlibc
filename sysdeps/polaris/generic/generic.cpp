#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <stdlib.h>
#include <abi-bits/fcntl.h>
#include <polaris/syscall.h>
#include <frg/hash.hpp>
#include <frg/hash_map.hpp>

namespace mlibc {

void sys_libc_log(const char *message) {
	syscall(SYS_puts, message);
}

void sys_exit(int status) {
	syscall(SYS_exit, status);
	__builtin_unreachable();
}

void sys_libc_panic() {
	sys_libc_log("\nmlibc: panic!\n");
	sys_exit(1);
	__builtin_unreachable();
}

int sys_tcb_set(void *pointer) {
	// ARCH_SET_FS
	syscall(SYS_prctl, 0x1002, pointer);
	return 0;
}

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	(void)time;
	syscall(SYS_futex, pointer, 0, expected);
	return 0;
}

int sys_futex_wake(int *pointer) {
	syscall(SYS_futex, pointer, 1, 0);
	return 0;
}

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
	*result = (int)syscall(SYS_ioctl, fd, request, arg);
	return 0;
}

int sys_isatty(int fd) {
	struct winsize ws;
	int ret;

	if (!sys_ioctl(fd, TIOCGWINSZ, &ws, &ret))
		return 0;

	return ENOTTY;
}

int sys_getcwd(char *buffer, size_t size) {
	syscall(SYS_getcwd, buffer, size);
	return 0;
}

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	int ret = syscall(SYS_openat, dirfd, path, flags, mode);
	if (ret < 0) {
		*fd = -1;
		return -ret;
	}
	*fd = (int)ret;
	return 0;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	return sys_openat(AT_FDCWD, path, flags, mode, fd);
}

#ifndef MLIBC_BUILDING_RTLD

int sys_tcgetattr(int fd, struct termios *attr) {
	int ret;
	if (int r = sys_ioctl(fd, TCGETS, attr, &ret) != 0) {
		return r;
	}
	return 0;
}

int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
	int ret;
	switch (optional_action) {
		case TCSANOW:
			optional_action = TCSETS; break;
		case TCSADRAIN:
			optional_action = TCSETSW; break;
		case TCSAFLUSH:
			optional_action = TCSETSF; break;
		default:
			__ensure(!"Unsupported tcsetattr");
	}

	if (int r = sys_ioctl(fd, optional_action, (void *)attr, &ret) != 0) {
		return r;
	}

	return 0;
}

int sys_open_dir(const char *path, int *handle) {
	return sys_openat(AT_FDCWD, path, O_DIRECTORY, 0, handle);
}

struct ReadDirState {
	size_t offset;
	size_t capacity;
	void *buffer;
};

static frg::hash_map<int, ReadDirState *, frg::hash<int>, MemoryAllocator> open_dirs{frg::hash<int>{}, getAllocator()};

static ReadDirState *get_dir_state(int fdnum) {
	ReadDirState *result;
	if (auto value = open_dirs.get(fdnum)) {
		result = *value;
	} else {
		result = (ReadDirState *)malloc(sizeof(ReadDirState));
		result->offset = 0;
		result->capacity = 1024;
		result->buffer = malloc(result->capacity);
		open_dirs.insert(fdnum, result);
	}
	return result;
}

int sys_read_entries(int fdnum, void *buffer, size_t max_size, size_t *bytes_read) {
	ReadDirState *state = get_dir_state(fdnum);

retry:
	uint64_t ret = syscall(SYS_readdir, fdnum, state->buffer, &state->capacity);
	int ret_but_int = (int)ret;
	if (ret_but_int < 0) {
		if (-ret_but_int == ENOBUFS) {
			state->buffer = realloc(state->buffer, state->capacity);
			goto retry;
		} else {
			return -ret_but_int;
		}
	}

	size_t offset = 0;
	while (offset < max_size) {
		struct dirent *ent = (struct dirent *)((char *)state->buffer + state->offset);
		if (ent->d_reclen == 0) {
			break;
		}

		if (offset + ent->d_reclen >= max_size) {
			break;
		}

		memcpy((char *)buffer + offset, ent, ent->d_reclen);
		offset += ent->d_reclen;
		state->offset += ent->d_reclen;
	}

	*bytes_read = offset;
	return 0;
}

#endif

int sys_close(int fd) {
	syscall(SYS_close, fd);
#ifndef MLIBC_BUILDING_RTLD
	open_dirs.remove(fd);
#endif
	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	uint64_t ret = syscall(SYS_seek, fd, offset, whence);

	int ret_but_int = (int)ret;
	if (ret_but_int < 0) {
		*new_offset = -1;
		return -ret_but_int;
	}

	*new_offset = (off_t)ret;
	return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	*bytes_read = (ssize_t)syscall(SYS_read, fd, buf, count);
	int ret = *bytes_read;
	if (ret < 0) {
		*bytes_read = -1;
		return -ret;
	}
	return 0;
}

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	*bytes_written = (ssize_t)syscall(SYS_write, fd, buf, count);
	int ret = *bytes_written;
	if (ret < 0) {
		*bytes_written = -1;
		return -ret;
	}
	return 0;
}

int sys_readlink(const char *path, void *data, size_t max_size, ssize_t *length) {
	*length = (ssize_t)syscall(SYS_readlinkat, AT_FDCWD, path, data, max_size);
	int ret = *length;
	if (ret < 0) {
		*length = -1;
		return -ret;
	}
	return 0;
}

int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	syscall(SYS_linkat, olddirfd, old_path, newdirfd, new_path, flags);
	return 0;
}

int sys_link(const char *old_path, const char *new_path) {
	return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int sys_unlinkat(int fd, const char *path, int flags) {
	syscall(SYS_unlinkat, fd, path, flags);
	return 0;
}

int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
	syscall(SYS_fchmodat, fd, pathname, mode, flags);
	return 0;
}

int sys_fchmod(int fd, mode_t mode) {
	return sys_fchmodat(fd, "", mode, AT_EMPTY_PATH);
}

int sys_chmod(const char *pathname, mode_t mode) {
	int ret = sys_fchmodat(AT_FDCWD, pathname, mode, 0);
	ret = ret < 0 ? -ret : ret;
	return ret;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags,
			   int fd, off_t offset, void **window) {
	uint64_t ret = syscall(SYS_mmap, hint, size, prot, flags, fd, offset);
	int ret_but_int = (int)ret;
	if (ret_but_int < 0) {
		*window = ((void *)-1);
		return -ret_but_int;
	}
	*window = (void *)ret;
	return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
	syscall(SYS_munmap, pointer, size);
	return 0;
}

int sys_anon_allocate(size_t size, void **pointer) {
	return sys_vm_map(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, -1, 0, pointer);
}

int sys_anon_free(void *pointer, size_t size) {
	return sys_vm_unmap(pointer, size);
}

#ifndef MLIBC_BUILDING_RTLD

pid_t sys_getpid() {
	return (pid_t)syscall(SYS_getpid);
}

pid_t sys_getppid() {
	return (pid_t)syscall(SYS_getppid);
}

uid_t sys_getuid() {
	return 0;
}

uid_t sys_geteuid() {
	return 0;
}

gid_t sys_getgid() {
	return 0;
}

int sys_setgid(gid_t) {
	return 0;
}

int sys_getpgid(pid_t, pid_t *) {
	return 0;
}

gid_t sys_getegid() {
	return 0;
}

int sys_setpgid(pid_t, pid_t) {
	return 0;
}

int sys_ttyname(int, char *, size_t) {
	return ENOSYS;
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	struct timespec buf;
	syscall(SYS_getclock, clock, &buf);
	*secs = buf.tv_sec;
	*nanos = buf.tv_nsec;
	return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	switch (fsfdt) {
		case fsfd_target::fd:
			syscall(SYS_fstatat, fd, "", statbuf, flags | AT_EMPTY_PATH);
			break;
		case fsfd_target::path:
			syscall(SYS_fstatat, AT_FDCWD, path, statbuf, flags);
			break;
		case fsfd_target::fd_path:
			syscall(SYS_fstatat, fd, path, statbuf, flags);
			break;
		default:
			__ensure(!"sys_stat: Invalid fsfdt");
			__builtin_unreachable();
	}
	return 0;
}

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
	(void)flags;
	struct stat buf;
	if (int r = sys_stat(fsfd_target::fd_path, dirfd, pathname, mode & AT_SYMLINK_FOLLOW, &buf)) {
		return r;
	}
	return 0;
}

int sys_access(const char *path, int mode) {
	return sys_faccessat(AT_FDCWD, path, mode, 0);
}

int sys_pipe(int *fds, int flags) {
	int ret = syscall(SYS_pipe, fds, flags);
	if (ret < 0) {
		return -ret;
	}
	return 0;
}

int sys_chdir(const char *path) {
	syscall(SYS_chdir, path);
	return 0;
}

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	syscall(SYS_mkdirat, dirfd, path, mode);
	return 0;
}

int sys_mkdir(const char *path, mode_t mode) {
	return sys_mkdirat(AT_FDCWD, path, mode);
}

int sys_inotify_create(int, int *) {
	mlibc::infoLogger() << "mlibc: sys_inotify_create() is unimplemented" << frg::endlog;
	return ENOSYS;
}

int sys_fork(pid_t *child) {
	*child = (pid_t)syscall(SYS_fork);
	return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	int ret = syscall(SYS_execve, path, argv, envp);
	ret = ret < 0 ? -ret : 0;
	return ret;
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
	*result = (int)syscall(SYS_fcntl, fd, request, va_arg(args, uint64_t));
	int ret = *result;
	if (ret < 0) {
		*result = ret;
		return -ret;
	}
	return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
	(void)flags;
	*newfd = (int)syscall(SYS_fcntl, fd, F_DUPFD, 0);
	int ret = *newfd;
	if (ret < 0) {
		*newfd = -1;
		return -ret;
	}
	return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
	syscall(SYS_dup3, fd, newfd, flags);
	return 0;
}

int sys_sigprocmask(int, const sigset_t *__restrict, sigset_t *__restrict) {
    	mlibc::infoLogger() << "mlibc: sys_sigprocmask() is a stub\n" << frg::endlog;
    	return 0;
}

int sys_sigaction(int, const struct sigaction *, struct sigaction *) {
    	mlibc::infoLogger() << "mlibc: sys_sigaction() is a stub\n" << frg::endlog;
    	return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
    	if (ru) {
        	mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported\n" << frg::endlog;
        	return ENOSYS;
	}

	int ret = syscall(SYS_waitpid, pid, status, flags);
	if (ret < 0) {
		*ret_pid = -1;
		return -ret;
	}
    
	*ret_pid = ret;
    	return 0;
}

int sys_getgroups(size_t, const gid_t *, int *) {
	mlibc::infoLogger() << "mlibc: sys_getgroups() is unimplemented" << frg::endlog;
	return ENOSYS;
}

int sys_gethostname(char *buffer, size_t bufsize) {
	struct utsname utsname;
	if (int err = sys_uname(&utsname)) {
		return err;
	}
	if (strlen(utsname.nodename) >= bufsize) {
		return ENAMETOOLONG;
	}
	strncpy(buffer, utsname.nodename, bufsize);
	return 0;
}

int sys_getitimer(int, struct itimerval *) {
	mlibc::infoLogger() << "mlibc: sys_getitimer() is unimplemented" << frg::endlog;
	return ENOSYS;
}

int sys_setitimer(int, const struct itimerval *, struct itimerval *) {
	mlibc::infoLogger() << "mlibc: sys_setitimer() is unimplemented" << frg::endlog;
	return ENOSYS;
}

int sys_uname(struct utsname *buf) {
    	return syscall(0x3f, buf);
}

int sys_fsync(int) {
	mlibc::infoLogger() << "sys_fsync is a stub" << frg::endlog;
	return 0;
}

#endif

}
