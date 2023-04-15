#include <bits/ensure.h>
#include <dirent.h>
#include <errno.h>
#include <fs.h>
#include <limits.h>
#include <linux/fb.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/thread-entry.hpp>
#include <string.h>
#include <sys/types.h>
#include <syscall.h>
#include <unistd.h>

namespace mlibc {

void sys_libc_log(const char *message) {
	int ret;
	int errno;
	SYSCALL_NA1(SYS_LOG_LIBC, message);

	return;
}

void sys_libc_panic() {
	mlibc::infoLogger() << "\e[31mmlibc: panic!" << frg::endlog;
	for (;;)
		;
}

int sys_tcb_set(void *pointer) {
	int ret;
	int errno;
	SYSCALL_NA1(SYS_TCB_SET, pointer);

	mlibc::infoLogger() << "Got tcb_set return " << ret << frg::endlog;
	return ret;
}

int sys_anon_allocate(size_t size, void **pointer) {
	int errno = sys_vm_map(
		NULL,
		size,
		PROT_EXEC | PROT_READ | PROT_WRITE,
		MAP_ANONYMOUS,
		-1,
		0,
		pointer
	);

	return errno;
}

int sys_anon_free(void *pointer, size_t size) {
	// TODO
	return 0;
}

#ifndef MLIBC_BUILDING_RTDL
void sys_exit(int status) {
	int ret;
	int errno;
	SYSCALL_NA1(SYS_EXIT, status);
	__builtin_unreachable();
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_clock_get(int clock, time_t *secs, long *nanos) {
	mlibc::infoLogger() << "[mlibc] Calling sys_clock_get() STUB\n" << frg::endlog;

	uint64_t ret;
	int errno;
	SYSCALL_NA0(SYS_CLOCK);
	*secs = ret / 1000;
	*nanos = (ret % 1000) * 1000000;

	return 0;
}
#endif

int sys_open(const char *path, int flags, unsigned int mode, int *fd) {
	mlibc::infoLogger() << "[mlibc] Called sys_open on " << path << frg::endlog;
	mlibc::infoLogger() << "[mlibc] path len is " << strlen(path) << frg::endlog;
	// if(path[0] == '\0' ||  path == NULL || strlen(path) == 0 || strlen(path) ==
	// 1) return EINVAL;

	int ret;
	int errno;
	SYSCALL_NA2(SYS_OPEN, path, flags);

	if (ret == -1)
		return errno;

	*fd = ret;
	mlibc::infoLogger() << "[mlibc] Got sys_open return value " << *fd << "\n" << frg::endlog;

	return 0;
}

int sys_open_dir(const char *path, int *handle) {
	mlibc::infoLogger() << "[mlibc] sys_open_dir; path: " << path << "\n" << frg::endlog;
	return sys_open(path, O_DIRECTORY | O_RDONLY, 0, handle);
}

int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	__ensure(max_size >= sizeof(dirent));

	mlibc::infoLogger() << "[mlibc] sys_read_entries STUB; handle: " << handle << "\n"
			    << frg::endlog;

	int ret;
	int errno;
	SYSCALL_NA3(SYS_READDIR, handle, buffer, max_size);

	dirent ret_entry;
	DirectoryEntry *entry = (DirectoryEntry *) buffer;

	if (ret == -1) {
		*bytes_read = 0;
		return 0;
	}

	ret_entry.d_ino = entry->d_ino;
	ret_entry.d_off = 0;
	ret_entry.d_reclen = sizeof(DirectoryEntry);
	memcpy(ret_entry.d_name, entry->d_name, 128);

	mlibc::infoLogger() << "Got entry " << entry->d_name << "\n" << frg::endlog;
	switch (entry->d_type) {
	case VFS_DIRECTORY:
		ret_entry.d_type = DT_DIR;
		mlibc::infoLogger() << "Type is DIR"
				    << "\n"
				    << frg::endlog;
		break;
	case VFS_FILE:
		ret_entry.d_type = DT_REG;
		mlibc::infoLogger() << "Type is FILE"
				    << "\n"
				    << frg::endlog;
		break;
	case VFS_CHARDEVICE:
		ret_entry.d_type = DT_CHR;
		break;
	default:
		ret_entry.d_type = DT_UNKNOWN;
		mlibc::infoLogger() << "Type is UNKNOWN"
				    << "\n"
				    << frg::endlog;
		break;
	}

	memcpy(buffer, &ret_entry, sizeof(dirent));
	*bytes_read = ret_entry.d_reclen;

	return 0;
}

int sys_close(int fd) {
	mlibc::infoLogger() << "[mlibc] Close was called on " << fd << "\n" << frg::endlog;

	return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	mlibc::infoLogger() << "[mlibc] SYS_READ read called" << fd << " bytes";

	ssize_t ret;
	int errno;
	SYSCALL_NA3(SYS_READ, fd, buf, count);

	if (ret == -1)
		return errno;

	*bytes_read = ret;

	mlibc::infoLogger() << "[mlibc] SYS_READ read " << *bytes_read << " bytes" << frg::endlog;

	return 0;
}

#ifndef MLIBC_BUILDING_RTDL
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	mlibc::infoLogger() << "[mlibc] Write was called on " << fd << "\n" << frg::endlog;
	size_t ret;
	int errno;
	SYSCALL_NA3(SYS_WRITE, fd, buf, count);

	if (ret == (size_t) -1)
		return errno;
	*bytes_written = ret;
	mlibc::infoLogger() << *bytes_written << " bytes written\n" << frg::endlog;

	return 0;
}
#endif

int sys_chdir(const char *path) {
	int ret;
	int errno;
	SYSCALL_NA1(SYS_CHDIR, path);

	if (ret == -1)
		return errno;

	return 0;
}

int sys_getcwd(char *buffer, size_t size) {
	int ret;
	int errno;
	SYSCALL_NA2(SYS_GETCWD, buffer, size);

	if (ret == -1)
		return errno;

	return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	// TODO

	if (fd == 0 || fd == 1 || fd == 2)
		return ESPIPE;
	int ret;
	int errno;
	SYSCALL_NA3(SYS_SEEK, fd, offset, whence);

	if (ret == -1)
		return EINVAL;

	if (ret < 0)
		return EOVERFLOW;

	*new_offset = ret;

	mlibc::infoLogger() << "[mlibc] Sys seek got ret " << *new_offset << "\n" << frg::endlog;

	return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	mlibc::infoLogger() << "[mlibc] Calling sys_stat() STUB\n" << frg::endlog;

	VFSNodeStat vns;
	switch (fsfdt) {
	case fsfd_target::fd: {
		mlibc::infoLogger() << "[mlibc] Calling fstat on " << fd << " fd\n" << frg::endlog;

		int ret;
		int errno;
		SYSCALL_NA2(SYS_FSTAT, fd, &vns);

		if (ret == -1)
			return EBADF;
		break;
	}
	case fsfd_target::path: {
		mlibc::infoLogger() << "[mlibc] Calling stat on " << path << "\n" << frg::endlog;

		int ret;
		int errno;
		SYSCALL_NA2(SYS_STAT, path, &vns);
		if (ret == -1) {
			mlibc::infoLogger() << "[mlibc] did not get statbuf for " << path << "\n"
					    << frg::endlog;
			return ENOENT;
		}
		break;
	}
	default: {
		mlibc::infoLogger() << "Invalid fsfd_target \n" << frg::endlog;
		return EBADF;
		break;
	}
	}

	memset(statbuf, 0, sizeof(struct stat));

	statbuf->st_ino = vns.inode;
	statbuf->st_size = vns.filesize;
	statbuf->st_dev = 0;
	statbuf->st_nlink = 1;
	statbuf->st_uid = 0;
	statbuf->st_gid = 0;
	statbuf->st_rdev = 0;
	statbuf->st_blksize = 0;
	statbuf->st_blocks = 0;
	statbuf->st_mode = 0;

	vtype_t type = vns.type;

	switch (type) {
	case VFS_FILE:
		mlibc::infoLogger() << "[mlibc] Regular file " << path << "\n" << frg::endlog;
		statbuf->st_mode |= S_IFREG;
		break;
	case VFS_DIRECTORY:
		mlibc::infoLogger() << "[mlibc] Regular directory " << path << "\n" << frg::endlog;
		statbuf->st_mode |= S_IFDIR;
		break;
	case VFS_CHARDEVICE:
		mlibc::infoLogger() << "[mlibc] Chardevice " << path << "\n" << frg::endlog;
		statbuf->st_mode |= S_IFCHR;
		statbuf->st_dev = vns.rdev;
		break;
	default:
		mlibc::panicLogger() << "[mlibc] unknown type " << type << "\n" << frg::endlog;
		sys_libc_panic();
	}

	mlibc::infoLogger() << "[mlibc] size: " << statbuf->st_size << " inode: " << statbuf->st_ino
			    << "\n"
			    << frg::endlog;

	return 0;
}

int sys_access(const char *filename, int mode) {
	mlibc::infoLogger() << "[mlibc] Calling sys_access() STUB\n" << frg::endlog;

	int ret;
	int errno;

	SYSCALL_NA2(SYS_ACCESS, filename, mode);

	if (ret == -1)
		return errno;

	return ret;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	// __ensure(flags & MAP_ANONYMOUS);

	void *ret;
	int errno;
	SYSCALL_NA6(SYS_VM_MAP, hint, size, prot, flags, fd, offset);

	mlibc::infoLogger() << "[mlibc] Derefing window at " << window << "\n" << frg::endlog;
	*window = (void *) ret;
	mlibc::infoLogger() << "[mlibc] Got vm_map return value " << *window << frg::endlog;

	return 0;
}

int sys_vm_unmap(void *pointer, size_t size) {
	// TODO
	return sys_anon_free(pointer, size);
}

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	mlibc::infoLogger() << "[mlibc] Calling sys_futex_wait() STUB\n" << frg::endlog;

	return 0;
}

int sys_isatty(int fd) {
	mlibc::infoLogger() << "[mlibc] Called sys_isatty on fd " << fd << "\n" << frg::endlog;
	return 0;
}

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
	mlibc::infoLogger() << "[mlibc] Called sys_ioctl(); fd: " << fd << "\n" << frg::endlog;

	int ret;
	int errno;
	SYSCALL_NA3(SYS_IOCTL, fd, request, arg);

	if (ret == -1)
		return errno;

	*result = ret;
	return 0;
}

int sys_futex_wake(int *pointer) {
	mlibc::infoLogger() << "[mlibc] Calling sys_futex_wake() STUB\n" << frg::endlog;

	return 0;
}

// All remaining functions are disabled in ldso.
#ifndef MLIBC_BUILDING_RTDL

int sys_clone(void *entry, void *user_arg, void *tcb, pid_t *tid_out) {
	mlibc::infoLogger() << "[mlibc] Calling sys_clone() STUB\n" << frg::endlog;
	return 0;
}

int sys_tcgetattr(int fd, struct termios *attr) {
	// return sys_ioctl(fd, TCGETS, (void *)attr, &res);
	return ENOSYS;
}

int sys_tcsetattr(int fd, int opts, const struct termios *attr) {
	return ENOSYS;
}

void sys_thread_exit() {
	mlibc::panicLogger() << "[mlibc] Calling sys_thread_exit() STUB\n" << frg::endlog;
	__builtin_trap();
}

int sys_sleep(time_t *secs, long *nanos) {
	mlibc::infoLogger() << "[mlibc] Calling sys_sleep() STUB\n" << frg::endlog;
	return 0;
}

int sys_fork(pid_t *child) {
	mlibc::infoLogger() << "[mlibc] Calling sys_fork() \n" << frg::endlog;

	pid_t ret;
	int errno;
	SYSCALL_NA0(SYS_FORK);

	if (ret == -1)
		return ENOMEM;

	*child = ret;

	return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	mlibc::infoLogger() << "[mlibc] Calling sys_execve() STUB\n" << frg::endlog;

	int ret;
	int errno;
	SYSCALL_NA3(SYS_EXEC, path, argv, envp);
	if (ret == -1)
		return errno;

	return 0;
}

uid_t sys_getuid() {
	mlibc::infoLogger() << "[mlibc] Calling sys_getuid() STUB\n" << frg::endlog;
	return 1000;
}

gid_t sys_getgid() {
	mlibc::infoLogger() << "[mlibc] Calling sys_getgid() STUB\n" << frg::endlog;
	return 1000;
}

uid_t sys_geteuid() {
	mlibc::infoLogger() << "[mlibc] Calling sys_geteuid() STUB\n" << frg::endlog;
	return 1000;
}

gid_t sys_getegid() {
	mlibc::infoLogger() << "[mlibc] Calling sys_getegid() STUB\n" << frg::endlog;
	return 1000;
}

pid_t sys_getpid() {
	mlibc::infoLogger() << "[mlibc] Calling sys_getpid() \n" << frg::endlog;

	pid_t ret;
	int errno;
	SYSCALL_NA0(SYS_GETPID)

	pid_t res = ret;
	mlibc::infoLogger() << "[mlibc] Got pid: " << res << frg::endlog;
	return ret;
}

int sys_getpgid(pid_t pid, pid_t *pgid) {
	mlibc::infoLogger() << "[mlibc] Calling sys_getpgid() STUB\n" << frg::endlog;
	*pgid = 0;
	return 0;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	mlibc::infoLogger() << "[mlibc] Calling sys_setpgid() STUB\n" << frg::endlog;
	return ENOSYS;
}

pid_t sys_getppid() {
	mlibc::infoLogger() << "[mlibc] Calling sys_getppid() STUB\n" << frg::endlog;
	return ENOSYS;
}

int sys_dup(int fd, int flags, int *newfd) {
	mlibc::infoLogger() << "[mlibc] Calling sys_dup() \n" << frg::endlog;

	int ret;
	int errno;
	SYSCALL_NA2(SYS_DUP, fd, flags);

	if (ret)
		*newfd = ret;

	mlibc::infoLogger() << "[mlibc] Calling sys_dup() \n" << frg::endlog;

	return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
	mlibc::infoLogger() << "[mlibc] Calling sys_dup2() \n" << frg::endlog;

	int ret;
	int errno;
	SYSCALL_NA3(SYS_DUP2, fd, flags, newfd);
	if (ret != newfd)
		return -1;
	mlibc::infoLogger() << "[mlibc] sys_dup2(): success \n" << frg::endlog;

	return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	mlibc::infoLogger() << "[mlibc] Calling sys_waitpid()\n" << frg::endlog;

	if (ru) {
		mlibc::infoLogger() << "[mlibc] waitpid(): rusage not supported\n" << frg::endlog;
		return EINVAL;
	}

	pid_t ret = 0;
	int errno = 0;
again:
	SYSCALL_NA3(SYS_WAIT, pid, status, flags);

	if (ret == -1) {
		if (errno == EINTR) {
			goto again;
		} else
			return errno;
	}

	*ret_pid = ret;
	return 0;
}

int sys_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	mlibc::infoLogger() << "[mlibc] Calling sys_sigaction() STUB; Signal:" << signum << " \n"
			    << frg::endlog;
	return ENOSYS;

	switch (signum) {
	case SIGCHLD: {
		mlibc::infoLogger() << "[mlibc] SIGCHLD"
				    << " \n"
				    << frg::endlog;
		*oldact = *act;
		break;
	}
	case SIGUSR1: {
		mlibc::infoLogger() << "[mlibc] SIGUSR1"
				    << " \n"
				    << frg::endlog;
		*oldact = *act;
		break;
	}
	case SIGUSR2: {
		mlibc::infoLogger() << "[mlibc] SIGUSR2"
				    << " \n"
				    << frg::endlog;
		*oldact = *act;
		break;
	}
	default:
		break;
	}

	return 0;
}

int sys_pipe(int *fds, int flags) {
	mlibc::infoLogger() << "[mlibc] Calling sys_pipe() STUB\n" << frg::endlog;
	return 0;
}

int sys_fcntl(int fd, int request, va_list args, int *result_value) {
	mlibc::infoLogger() << "[mlibc] Calling sys_fcntl() on " << fd << "\n" << frg::endlog;
	return ENOSYS;

	switch (request) {
	case F_DUPFD:
		mlibc::infoLogger() << "[mlibc] REQ: F_DUPFD "
				    << "\n"
				    << frg::endlog;
		return sys_dup(fd, 0, result_value);
		break;
	case F_DUPFD_CLOEXEC:
		mlibc::infoLogger() << "[mlibc] REQ: F_DUPFD_CLOEXEC "
				    << "\n"
				    << frg::endlog;
		break;
	case F_SETFD:
		mlibc::infoLogger() << "[mlibc] REQ: F_SETFD " << fd << "\n" << frg::endlog;

		if (fd < 255) {
			mlibc::infoLogger() << "[mlibc] Calling sys_fcntl() on " << fd << "\n"
					    << frg::endlog;
			*result_value = 0;
			return fd;
		} else {
			mlibc::infoLogger() << "[mlibc] Returning EBADF"
					    << "\n"
					    << frg::endlog;
			return EBADF;
		}
		break;
	case F_GETFD:
		mlibc::infoLogger() << "[mlibc] REQ: F_GETFD "
				    << "\n"
				    << frg::endlog;
		break;

	case F_SETFL:
		mlibc::infoLogger() << "[mlibc] REQ: F_SETFL "
				    << "\n"
				    << frg::endlog;
		break;
	case F_GETFL:
		mlibc::infoLogger() << "[mlibc] REQ: F_GETFL "
				    << "\n"
				    << frg::endlog;
		break;
	default:
		sys_libc_panic();
	}

	return ENOSYS;
}

int sys_gethostname(char *buffer, size_t bufsize) {
	if (bufsize < 4)
		return EINVAL;

	strcpy(buffer, "root");

	return 0;
}

#endif  // MLIBC_BUILDING_RTDL

int sys_mkdir(const char *path, mode_t mode) {
	int ret;
	int errno;
	SYSCALL_NA2(SYS_MKDIR, path, mode);
	if (ret == -1)
		return errno;
	return 0;
}

int sys_poll(struct pollfd *fds, nfds_t cnt, int timeout, int *events) {
	if (!fds || !events)
		return EINVAL;

	int ret;
	int errno;
	SYSCALL_NA4(SYS_POLL, fds, cnt, timeout, events);

	if (ret == -1)
		return errno;

	return 0;
}

}  // namespace mlibc

