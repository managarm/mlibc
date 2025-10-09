#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sched.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/file.h>
#include <mlibc/tcb.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stddef.h>

extern "C" void __mlibc_sigret(void);

namespace mlibc {

void sys_libc_log(const char *message) {
	ssize_t unused;
	sys_write(1, message, strlen(message), &unused);
	sys_write(1, "\n", 1, &unused);
}

void sys_libc_panic() {
	ssize_t unused;
	char const *message = "mlibc panicked unrecoverably\n";
	sys_write(2, message, strlen(message), &unused);
	sys_exit(1);
}

void sys_exit(int status) {
	int ret, errno;
	SYSCALL1(SYSCALL_EXIT, status);
	__builtin_unreachable();
}

int sys_tcb_set(void *pointer) {
	int errno;

#if defined(__x86_64__)
	int ret;
	SYSCALL2(SYSCALL_ARCH_PRCTL, 1, pointer);
#elif defined(__riscv) && __riscv_xlen == 64
	uintptr_t tp = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile("mv tp, %0" : : "r"(tp) : "memory");
	errno = 0;
#else
	#error Unknown architecture
#endif

	return errno;
}

int sys_thread_getname(void *tcb, char *name, size_t size) {
	int ret, errno;
	auto t = reinterpret_cast<Tcb *>(tcb);
	SYSCALL3(SYSCALL_GETTIDID, t->tid, name, size);
	return errno;
}

int sys_thread_setname(void *tcb, const char *name) {
	int ret, errno;
	size_t len = strlen(name);
	auto t = reinterpret_cast<Tcb *>(tcb);
	SYSCALL3(SYSCALL_SETTIDID, t->tid, name, len);
	return errno;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	return sys_openat(AT_FDCWD, path, flags, mode, fd);
}

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	int ret, errno;

	int path_len = strlen(path);
	SYSCALL4(SYSCALL_OPEN, dirfd, path, path_len, flags);

	// Have to check for O_CREAT since there is a pesky Linux-specific O_EXCL
	// extension that makes it not fail if we are opening a block device.
	// Otherwise O_EXCL with the file existing should be always a failure.
	if (ret != -1 && (flags & O_EXCL) && (flags & O_CREAT)) {
		SYSCALL1(SYSCALL_CLOSE, ret);
		return EEXIST;
	}

	// We implement creating files in this sysdep.
	if ((errno == ENOENT) && (flags & O_CREAT) && ((flags & O_DIRECTORY) == 0)) {
		SYSCALL5(SYSCALL_MAKENODE, AT_FDCWD, path, path_len, S_IFREG | mode, 0);
		if (ret == -1) {
			return errno;
		}
		SYSCALL4(SYSCALL_OPEN, AT_FDCWD, path, path_len, flags);
	}

	// Handle some post-opening triggers.
	if (ret != -1) {
		if (flags & O_TRUNC) {
			// If the file cannot be truncated, dont sweat it, some software
			// depends on some things being truncate-able that ironclad does
			// not allow. For example, some devices.
			sys_ftruncate(ret, 0);
		}
		if (flags & O_DIRECTORY) {
			struct stat st;
			sys_stat(fsfd_target::fd, ret, NULL, 0, &st);
			if (!S_ISDIR (st.st_mode)) {
				SYSCALL1(SYSCALL_CLOSE, ret);
				ret	= -1;
				errno = ENOTDIR;
			}
		}
	}

	*fd = ret;
	return errno;
}

int sys_open_dir(const char *path, int *handle) {
	return sys_open(path, O_RDONLY | O_DIRECTORY, 0660, handle);
}

int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	size_t ret;
	int errno;
	SYSCALL3(SYSCALL_GETDENTS, handle, buffer, max_size);
	if (errno != 0) {
		return errno;
	} else {
		*bytes_read = ret;
		return 0;
	}
}

void sys_thread_exit() {
	 int ret, errno;
	 SYSCALL0(SYSCALL_EXIT_THREAD);
	 __builtin_unreachable();
}

int sys_close(int fd) {
	int ret, errno;
	SYSCALL1(SYSCALL_CLOSE, fd);
	return errno;
}

void sys_sync() {
	int ret, errno;
	SYSCALL0(SYSCALL_SYNC);
	if (ret != 0) {
		sys_libc_log("mlibc: sync failed");
	}
}

int sys_fsync(int fd) {
	int ret, errno;
	SYSCALL2(SYSCALL_FSYNC, fd, 0);
	return errno;
}

int sys_fdatasync(int fd) {
	int ret, errno;
	SYSCALL2(SYSCALL_FSYNC, fd, 1);
	return errno;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	ssize_t ret;
	int errno;
	SYSCALL5(SYSCALL_READ, fd, buf, count, 0, 0);
	*bytes_read = ret;
	return errno;
}

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	ssize_t ret;
	int errno;
	SYSCALL5(SYSCALL_WRITE, fd, buf, count, 0, 0);
	*bytes_written = ret;
	return errno;
}

int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	ssize_t ret;
	int errno;
	SYSCALL5(SYSCALL_READ, fd, buf, n, off, 1);
	*bytes_read = ret;
	return errno;
}

int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
	ssize_t ret;
	int errno;
	SYSCALL5(SYSCALL_WRITE, fd, buf, n, off, 1);
	*bytes_written = ret;
	return errno;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	ssize_t ret;
	int errno;
	SYSCALL3(SYSCALL_SEEK, fd, offset, whence);
	*new_offset = ret;
	return errno;
}

int sys_ftruncate (int fd, size_t size) {
	int ret, errno;
	SYSCALL2(SYSCALL_TRUNCATE, fd, size);
	return errno;
}

int sys_fallocate(int fd, off_t offset, size_t size) {
	if (offset < 0 || size == 0) {
		return EINVAL;
	}

	int ret, errno;
	SYSCALL2(SYSCALL_TRUNCATE, fd, offset + size);
	return errno;
}

int sys_flock(int fd, int options) {
	struct flock lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = (off_t)((uint64_t)-1);
	lock.l_pid = sys_getpid();

	switch (options & ~(LOCK_NB)) {
		case LOCK_SH:
			lock.l_type = F_RDLCK;
			break;
		case LOCK_EX:
			lock.l_type = F_WRLCK;
			break;
		case LOCK_UN:
			lock.l_type = F_UNLCK;
			break;
		default:
			return -1;
	}

	int command = options & LOCK_NB ? F_SETLK : F_SETLKW;
	int ret, errno;
	SYSCALL3(SYSCALL_FCNTL, fd, command, &lock);
	return errno;
}

int sys_getpriority(int which, id_t who, int *value) {
	int ret, errno;
	SYSCALL2(SYSCALL_GETPRIO, which, who);
	*value = ret;
	return errno;
}

int sys_setpriority(int which, id_t who, int value) {
	int ret, errno;
	SYSCALL3(SYSCALL_SETPRIO, which, who, value);
	return errno;
}

int sys_getrusage(int scope, struct rusage *usage) {
	int ret, errno;
	SYSCALL2(SYSCALL_GETRUSAGE, scope, usage);

	// Ironclad returns nanoseconds instead of microseconds for usage, so we
	// have to compensate for that.
	usage->ru_utime.tv_usec = usage->ru_utime.tv_usec / 1000;
	usage->ru_stime.tv_usec = usage->ru_stime.tv_usec / 1000;

	return errno;
}

int sys_anon_allocate(size_t size, void **pointer) {
	return sys_vm_map(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON, 0, 0, pointer);
}

int sys_anon_free(void *pointer, size_t size) {
	return sys_vm_unmap(pointer, size);
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	void *ret;
	int errno;
	SYSCALL6(SYSCALL_MMAP, hint, size, prot, flags, fd, offset);
	*window = ret;

	if ((errno == ENOMEM) && ((flags & MAP_ANON) == 0)) {
		int ret = sys_anon_allocate(size, window);
		if (ret) {
			return ret;
		}
		ssize_t len;
		ret = sys_pread(fd, *window, size, offset, &len);
		return 0;
	}

	return errno;
}

int sys_posix_madvise(void *addr, size_t length, int advice) {
	int ret, errno;
	SYSCALL3(SYSCALL_MADVISE, addr, length, advice);
	return errno;
}

int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
	int ret, errno;
	SYSCALL5(SYSCALL_GETSOCKOPT, fd, layer, number, buffer, size);
	return errno;
}

int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
	int ret, errno;
	SYSCALL5(SYSCALL_SETSOCKOPT, fd, layer, number, buffer, size);
	return errno;
}

int sys_vm_unmap(void *pointer, size_t size) {
	int ret;
	int errno;
	SYSCALL2(SYSCALL_MUNMAP, pointer, size);
	if (ret != 0) {
		return errno;
	} else {
		return 0;
	}
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
	int ret;
	int errno;
	SYSCALL3(SYSCALL_MPROTECT, pointer, size, prot);
	if (ret != 0) {
		return errno;
	}
	return 0;
}

int sys_getsid(pid_t pid, pid_t *sid) {
	int ret, errno;
	SYSCALL1(SYSCALL_GETSID, pid);
	*sid = ret;
	return errno;
}

pid_t sys_getpid() {
	pid_t ret;
	int errno;
	SYSCALL0(SYSCALL_GETPID);
	return ret;
}

pid_t sys_gettid() {
	pid_t ret;
	int errno;
	SYSCALL0(SYSCALL_GETTID);
	return ret;
}

pid_t sys_getppid() {
	pid_t ret;
	int errno;
	SYSCALL0(SYSCALL_GETPPID);
	return ret;
}

int sys_getgroups(size_t size, gid_t *list, int *retval) {
	int ret, errno;
	SYSCALL2(SYSCALL_GETGROUPS, size, list);
	*retval = ret;
	return errno;
}

int sys_setgroups(size_t size, const gid_t *list) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETGROUPS, size, list);
	return errno;
}

int sys_ptrace(long req, pid_t pid, void *addr, void *data, long *out) {
	int ret, errno;
	SYSCALL4(SYSCALL_PTRACE, req, pid, addr, data);
	*out = (long)ret;
	return errno;
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
	int ret, errno;
	SYSCALL3(SYSCALL_FCNTL, fd, request, va_arg(args, uint64_t));
	*result = ret;
	return errno;
}

int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	int ret, errno;
	SYSCALL3(SYSCALL_SIGPROCMASK, how, set, retrieve);
	return errno;
}

int sys_sigaltstack(const stack_t *ss, stack_t *oss) {
	int ret, errno;
	SYSCALL2(SYSCALL_SIGALTSTACK, ss, oss);
	return errno;
}

int sys_tgkill(int pid, int tid, int sig) {
	(void)tid;
	return sys_kill(pid, sig);
}

int sys_isatty(int fd) {
	struct termios t;
	if (sys_tcgetattr(fd, &t) == 0) {
		return 0;
	} else {
		return ENOTTY;
	}
}

int sys_getpgid(pid_t pid, pid_t *pgid) {
	int ret, errno;
	SYSCALL1(SYSCALL_GETPGID, pid);
	*pgid = ret;
	return errno;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETPGID, pid, pgid);
	return errno;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	int ret, errno, argv_len, envp_len;
	for (argv_len = 0; argv[argv_len] != NULL; argv_len++);
	for (envp_len = 0; envp[envp_len] != NULL; envp_len++);

	size_t path_len = strlen (path);
	SYSCALL6(SYSCALL_EXEC, path, path_len, argv, argv_len, envp, envp_len);

	if (ret == -1) {
		return errno;
	 }

	return 0;
}

int sys_fork(pid_t *child) {
	pid_t ret;
	int errno;

	SYSCALL1(SYSCALL_FORK, 0);

	if (ret == -1) {
		return errno;
	}

	if (child != NULL) {
		*child = ret;
	}

	return 0;
}

int sys_vfork(pid_t *child) {
	pid_t ret;
	int errno;

	SYSCALL1(SYSCALL_FORK, 1);

	if (ret == -1) {
		return errno;
	}

	if (child != NULL) {
		*child = ret;
	}

	return 0;
}

int sys_getrlimit(int resource, struct rlimit *limit) {
	uint64_t ret, errno;
	SYSCALL3(SYSCALL_RLIMIT, resource, NULL, limit);
	return errno;
}

int sys_setrlimit(int resource, const struct rlimit *limit) {
	uint64_t ret, errno;
	SYSCALL3(SYSCALL_RLIMIT, resource, limit, NULL);
	return errno;
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	pid_t ret;
	int errno;

	if(ru) {
		mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported" << frg::endlog;
		return ENOSYS;
	}

	SYSCALL3(SYSCALL_WAIT, pid, status, flags);

	if (ret == -1) {
		return errno;
	}

	*ret_pid = ret;
	return errno;
}

int sys_uname(struct utsname *buf) {
	int ret, errno;
	SYSCALL1(SYSCALL_UNAME, buf);
	return errno;
}

int sys_ttyname(int fd, char *buff, size_t size) {
	int ret, errno;
	SYSCALL3(SYSCALL_TTYNAME, fd, buff, size);
	return errno;
}

int sys_ptsname(int fd, char *buff, size_t size) {
	return sys_ttyname(fd, buff, size);
}

int sys_sethostname(const char *buff, size_t size) {
	int ret, errno;

	SYSCALL2(SYSCALL_SETHOSTNAME, buff, size);

	if (ret == -1) {
		return errno;
	}

	return 0;
}

int sys_chdir(const char *buff) {
	int ret, errno;

	size_t buff_len = strlen(buff);
	SYSCALL4(SYSCALL_OPEN, AT_FDCWD, buff, buff_len, O_RDONLY);
	if (ret == -1) {
		return ENOENT;
	}

	SYSCALL1(SYSCALL_CHDIR, ret);

	if (ret == -1) {
		return errno;
	}

	return 0;
}

int sys_fchdir(int fd) {
	int ret, errno;

	SYSCALL1(SYSCALL_CHDIR, fd);

	if (ret == -1) {
		return errno;
	}

	return 0;
}

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
	int ret, errno;

	SYSCALL3(SYSCALL_IOCTL, fd, request, arg);

	if (ret == -1) {
		return errno;
	}

	*result = ret;
	return 0;
}

void sys_yield(void) {
	int ret, errno;
	SYSCALL0(SYSCALL_SCHED_YIELD);
}

int sys_getparam(pid_t pid, struct sched_param *param) {
	int ret, errno;
	SYSCALL2(SYSCALL_GET_SCHEDULER, pid, param);
	return errno;
}

int sys_setparam(pid_t pid, const struct sched_param *param) {
	int ret, errno;
	SYSCALL2(SYSCALL_SET_SCHEDULER, pid, param);
	return errno;
}

int sys_kill(int pid, int sig) {
	int ret, errno;
	SYSCALL2(SYSCALL_SEND_SIGNAL, pid, sig);
	return errno;
}

int sys_dup(int fd, int flags, int *newfd) {
	int ret, errno;
	if (flags & O_CLOEXEC) {
		SYSCALL3(SYSCALL_FCNTL, fd, F_DUPFD_CLOEXEC, 0);
	} else {
		SYSCALL3(SYSCALL_FCNTL, fd, F_DUPFD, 0);
	}

	*newfd = ret;

	if (errno == 0) {
		SYSCALL3(SYSCALL_FCNTL, *newfd, F_SETFD, flags);
	}

	return errno;
}

int sys_dup2(int fd, int flags, int newfd) {
	int ret = sys_close(newfd);
	if (ret != 0 && ret != EBADF) {
		return EBADF;
	}

	int errno;
	if (flags & O_CLOEXEC) {
		SYSCALL3(SYSCALL_FCNTL, fd, F_DUPFD_CLOEXEC, newfd);
	} else {
		SYSCALL3(SYSCALL_FCNTL, fd, F_DUPFD, newfd);
	}

	if (ret != -1 && ret != newfd) {
		return EBADF;
	}

	if (errno == 0) {
		SYSCALL3(SYSCALL_FCNTL, newfd, F_SETFD, flags);
	}

	return errno;
}

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

int sys_tcflow(int fd, int action) {
	int ret;
	return sys_ioctl(fd, TCXONC, &action, &ret);
}

int sys_tcflush(int fd, int action) {
	int ret;
	return sys_ioctl(fd, TCFLSH, &action, &ret);
}

int sys_tcdrain(int fd) {
	int ret;
	int value = 0;
	return sys_ioctl(fd, TCSBRKP, &value, &ret);
}

int sys_access(const char *path, int mode) {
	int ret, errno;
	size_t len = strlen(path);
	SYSCALL5(SYSCALL_ACCESS, AT_FDCWD, path, len, mode, 0);
	return errno;
}

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
	int ret, errno;
	size_t len = strlen(pathname);
	SYSCALL5(SYSCALL_ACCESS, dirfd, pathname, len, mode, flags);
	return errno;
}

struct futex_item {
	 uint64_t addr;
	 uint32_t expected;
	 uint32_t flags;
};

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	int ret, errno;
	struct futex_item item = {.addr = (uint64_t)pointer, .expected = (uint32_t)expected, .flags = 0};
	if (time == NULL) {
		 struct timespec t = {(time_t)-1, (time_t)-1};
		 SYSCALL4(SYSCALL_FUTEX, 0b01, &item, 1, &t);
	} else {
		 SYSCALL4(SYSCALL_FUTEX, 0b01, &item, 1, time);
	}
	return errno;
}

int sys_futex_wake(int *pointer) {
	int ret, errno;
	struct futex_item item = {.addr = (uint64_t)pointer, .expected = 0, .flags = 0};
	struct timespec t = {(time_t)-1, (time_t)-1};
	SYSCALL4(SYSCALL_FUTEX, 0b10, &item, 1, &t);
	return errno;
}

int sys_pipe(int *fds, int flags) {
	int ret, errno;
	SYSCALL2(SYSCALL_PIPE, fds, flags);
	return errno;
}

int sys_getentropy(void *buffer, size_t length) {
	ssize_t ret;
	int errno;
	SYSCALL2(SYSCALL_GETENTROPY, buffer, length);
	return errno;
}

int sys_mkdir(const char *path, mode_t mode) {
	return sys_mkdirat(AT_FDCWD, path, mode);
}

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	return sys_mknodat(dirfd, path, S_IFDIR | mode, 0);
}

int sys_rmdir(const char* path){
	int ret, errno;
	size_t path_len = strlen (path);
	SYSCALL4(SYSCALL_UNLINK, AT_FDCWD, path, path_len, AT_REMOVEDIR);
	return errno;
}

int sys_unlinkat(int fd, const char *path, int flags) {
	int ret, errno;
	size_t path_len = strlen (path);
	SYSCALL4(SYSCALL_UNLINK, fd, path, path_len, flags);
	return errno;
}

int sys_link(const char* srcpath, const char* destpath) {
	int ret, errno;
	size_t src_len = strlen (srcpath);
	size_t dst_len = strlen (destpath);
	SYSCALL6(SYSCALL_LINK, AT_FDCWD, srcpath, src_len, AT_FDCWD, destpath, dst_len);
	return errno;
}

int sys_socket(int domain, int type, int protocol, int *fd) {
	(void)protocol;
	int ret, errno;
	SYSCALL2(SYSCALL_SOCKET, domain, type);
	if (ret != -1) {
		*fd = ret;
		return 0;
	} else {
		return errno;
	}
}

int sys_socketpair(int domain, int type, int protocol, int *fds) {
	(void)protocol;
	int ret, errno;
	SYSCALL3(SYSCALL_SOCKETPAIR, domain, type, fds);
	return errno;
}

uid_t sys_getuid() {
	uint64_t ret, errno;
	SYSCALL0(SYSCALL_GETUID);
	return (uid_t)ret;
}

uid_t sys_geteuid() {
	uint64_t ret, errno;
	SYSCALL0(SYSCALL_GETEUID);
	return (uid_t)ret;
}

int sys_setuid(uid_t uid) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETUIDS, uid, uid);
	return ret;
}

int sys_seteuid(uid_t euid) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETUIDS, ((uint64_t)-1), euid);
	return ret;
}

int sys_setreuid(uid_t ruid, uid_t euid) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETUIDS, ruid, euid);
	return ret;
}

gid_t sys_getgid() {
	uint64_t ret, errno;
	SYSCALL0(SYSCALL_GETGID);
	return (gid_t)ret;
}

gid_t sys_getegid() {
	return sys_getgid();
}

int sys_setgid(gid_t gid) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETGIDS, gid, gid);
	return ret;
}

int sys_setegid(gid_t egid) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETGIDS, ((uint64_t)-1), egid);
	return ret;
}

int sys_setregid(gid_t rgid, gid_t egid) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETGIDS, rgid, egid);
	return ret;
}

int sys_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) {
	if (ruid) {
		*ruid = sys_getuid();
	}
	if (euid) {
		*euid = sys_geteuid();
	}
	if (suid) {
		*suid = sys_getuid();
	}
	return 0;
}

int sys_getresgid(uid_t *rgid, uid_t *egid, uid_t *sgid) {
	if (rgid) {
		*rgid = sys_getgid();
	}
	if (egid) {
		*egid = sys_getegid();
	}
	if (sgid) {
		*sgid = sys_getgid();
	}
	return 0;
}

int sys_setsid(pid_t *sid) {
	int ret, errno;
	SYSCALL0(SYSCALL_SETSID);
	*sid = ret;
	return errno;
}

#ifndef MLIBC_BUILDING_RTLD

extern "C" void __mlibc_thread_entry();

int sys_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	int ret, errno;

	if (act != NULL) {
		struct sigaction newact = *act;
		newact.sa_restorer = __mlibc_sigret;
		SYSCALL3(SYSCALL_SIGACTION, signum, &newact, oldact);
	} else {
		SYSCALL3(SYSCALL_SIGACTION, signum, NULL, oldact);
	}
	return errno;
}

int sys_clone(void *tcb, pid_t *tid_out, void *stack) {
	 int ret, errno;
	 SYSCALL4(SYSCALL_CREATE_THREAD, (uintptr_t)__mlibc_thread_entry, 0, stack, tcb);

	 if (ret == -1) {
		  return errno;
	 }

	 *tid_out = (pid_t)ret;
	 return 0;
}

int sys_prepare_stack(void **stack, void *entry, void *arg, void *tcb, size_t *stack_size, size_t *guard_size) {
	// TODO guard

	mlibc::infoLogger() << "mlibc: sys_prepare_stack() does not setup a guard!" << frg::endlog;

	*guard_size = 0;
	*stack_size = *stack_size ? *stack_size : 0x400000;

	if (!*stack) {
		*stack = (void *)((char *)mmap(NULL, *stack_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0) + *stack_size);
		if (*stack == MAP_FAILED) {
			return errno;
		}
	}

	void **stack_it = (void **)*stack;

	*--stack_it = arg;
	*--stack_it = tcb;
	*--stack_it = entry;

	*stack = (void *)stack_it;

	return 0;
}

int sys_clock_getres(int clock, time_t *secs, long *nanos) {
	struct timespec time;
	int ret, errno;
	SYSCALL3(SYSCALL_CLOCK, 0, clock, &time);
	*secs  = time.tv_sec;
	*nanos = time.tv_nsec;
	return errno;
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	struct timespec time;
	int ret, errno;
	SYSCALL3(SYSCALL_CLOCK, 1, clock, &time);
	*secs  = time.tv_sec;
	*nanos = time.tv_nsec;
	return errno;
}

int sys_clock_set(int clock, time_t secs, long nanos) {
	struct timespec time = {.tv_sec = secs, .tv_nsec = nanos };
	int ret, errno;
	SYSCALL3(SYSCALL_CLOCK, 2, clock, &time);
	return errno;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addrlen) {
	int ret, errno;
	SYSCALL3(SYSCALL_BIND, fd, addr_ptr, addrlen);
	return errno;
}

int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addrlen) {
	int ret, errno;
	SYSCALL3(SYSCALL_CONNECT, fd, addr_ptr, addrlen);
	return errno;
}

int sys_listen(int fd, int backlog) {
	int ret, errno;
	SYSCALL2(SYSCALL_LISTEN, fd, backlog);
	return errno;
}

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	int ret, errno;
	SYSCALL4(SYSCALL_ACCEPT, fd, addr_ptr, addr_length, flags);
	*newfd = ret;
	return errno;
}


int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
	int ret, errno;
	SYSCALL3(SYSCALL_GETSOCKNAME, fd, addr_ptr, &max_addr_length);
	*actual_length = max_addr_length;
	return errno;
}

int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
	int ret, errno;
	SYSCALL3(SYSCALL_GETPEERNAME, fd, addr_ptr, &max_addr_length);
	*actual_length = max_addr_length;
	return errno;
}

int sys_shutdown(int sockfd, int how) {
	int ret, errno;
	SYSCALL2(SYSCALL_SHUTDOWN, sockfd, how);
	return errno;
}

int sys_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value) {
	(void)which; (void)new_value; (void)old_value;
	return ENOSYS;
}

int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length) {
	int ret, errno;

	if (hdr->msg_control != NULL) {
		SYSCALL3(SYSCALL_RECVSOCKCTL, fd, hdr->msg_control, hdr->msg_controllen);
	}

	size_t count = 0;
	for (int i = 0; i < hdr->msg_iovlen; i++) {
		SYSCALL6(SYSCALL_RECVFROM, fd, hdr->msg_iov[i].iov_base, hdr->msg_iov[i].iov_len,
					flags, hdr->msg_name, hdr->msg_namelen);
		if (ret == -1) {
			return errno;
		}
		count += ret;
	}

	*length = count;
	return 0;
}

int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length) {
	int ret, errno;

	if (hdr->msg_control != NULL) {
		SYSCALL3(SYSCALL_SENDSOCKCTL, fd, hdr->msg_control, hdr->msg_controllen);
	}

	size_t count = 0;
	for (int i = 0; i < hdr->msg_iovlen; i++) {
		SYSCALL6(SYSCALL_SENDTO, fd, hdr->msg_iov[i].iov_base, hdr->msg_iov[i].iov_len,
					flags, hdr->msg_name, hdr->msg_namelen);
		if (ret == -1) {
			return errno;
		}
		count += ret;
	}

	*length = count;
	return 0;
}


int sys_ppoll(struct pollfd *fds, int nfds, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
	int ret, errno;
	SYSCALL4(SYSCALL_PPOLL, fds, nfds, timeout, sigmask);
	if (ret == -1) {
		return errno;
	}

	*num_events = ret;
	return errno;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	struct timespec ts;
	ts.tv_sec = timeout / 1000;
	ts.tv_nsec = (timeout % 1000) * 1000000;
	return sys_ppoll(fds, count, timeout == -1 ? NULL : &ts, NULL, num_events);
}

int sys_pause(void) {
	return sys_ppoll(NULL, 0, NULL, NULL, NULL);
}

int sys_sigsuspend(const sigset_t *set) {
	return sys_ppoll(NULL, 0, NULL, set, NULL);
}

int sys_pselect(int nfds, fd_set *read_set, fd_set *write_set,
		fd_set *except_set, const struct timespec *timeout,
		const sigset_t *sigmask, int *num_events) {
	struct pollfd *fds = (struct pollfd *)calloc(nfds, sizeof(struct pollfd));
	if (fds == NULL) {
		return ENOMEM;
	}

	for (int i = 0; i < nfds; i++) {
		struct pollfd *fd = &fds[i];

		if (read_set && FD_ISSET(i, read_set)) {
			fd->events |= POLLIN;
		}
		if (write_set && FD_ISSET(i, write_set)) {
			fd->events |= POLLOUT;
		}
		if (except_set && FD_ISSET(i, except_set)) {
			fd->events |= POLLPRI;
		}

		if (!fd->events) {
			fd->fd = -1;
			continue;
		}
		fd->fd = i;
	}

	int ret = sys_ppoll(fds, nfds, timeout, sigmask, num_events);
	if (ret != 0) {
		free(fds);
		return ret;
	}

	fd_set res_read_set, res_write_set, res_except_set;
	FD_ZERO(&res_read_set);
	FD_ZERO(&res_write_set);
	FD_ZERO(&res_except_set);

	for (int i = 0; i < nfds; i++) {
		struct pollfd *fd = &fds[i];

		if (read_set && FD_ISSET(i, read_set) && (fd->revents & (POLLIN | POLLERR | POLLHUP)) != 0) {
			FD_SET(i, &res_read_set);
		}
		if (write_set && FD_ISSET(i, write_set) && (fd->revents & (POLLOUT | POLLERR | POLLHUP)) != 0) {
			FD_SET(i, &res_write_set);
		}
		if (except_set && FD_ISSET(i, except_set) && (fd->revents & POLLPRI) != 0) {
			FD_SET(i, &res_except_set);
		}
	}

	free(fds);
	if (read_set) {
		*read_set = res_read_set;
	}
	if (write_set) {
		*write_set = res_write_set;
	}
	if (except_set) {
		*except_set = res_except_set;
	}

	return 0;
}

int sys_sleep(time_t *secs, long *nanos) {
	struct timespec time = {.tv_sec = *secs, .tv_nsec = *nanos};
	struct timespec rem  = {.tv_sec = 0, .tv_nsec = 0};

	int ret, errno;
	SYSCALL4(SYSCALL_CLOCK_NANOSLEEP, CLOCK_MONOTONIC, 0, &time, &rem);
	*secs  = rem.tv_sec;
	*nanos = rem.tv_nsec;
	return errno;
}

int sys_gethostname(char *buffer, size_t bufsize) {
	struct utsname buf;
	if (uname(&buf)) {
		return -1;
	}

	strncpy(buffer, buf.nodename, bufsize);
	return 0;
}

int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	int ret, errno;
	if (pathname == NULL) {
		pathname = "";
		flags |= AT_EMPTY_PATH;
	}

	size_t path_len = strlen (pathname);
	if (times == NULL) {
		 time_t secs;
		 long nsec;
		 ret = sys_clock_get(CLOCK_REALTIME, &secs, &nsec);
		 if (ret) {
			  return ret;
		 }
		 struct timespec times2[2] = {0, 0};
		 times2[0].tv_sec = secs;
		 times2[0].tv_nsec = nsec;
		 times2[1].tv_sec = secs;
		 times2[1].tv_nsec = nsec;
		 SYSCALL5(SYSCALL_UTIMES, dirfd, pathname, path_len, &times2[0], flags);
	} else {
		SYSCALL5(SYSCALL_UTIMES, dirfd, pathname, path_len, times, flags);
	}

	return errno;
}

struct meminfo {
	uint64_t phys_total;
	uint64_t phys_available;
	uint64_t phys_free;
	uint64_t shared_usage;
	uint64_t kernel_usage;
	uint64_t table_usage;
	uint64_t poison_usage;
};

struct cpuinfo {
	uint64_t conf_cores;
	uint64_t onln_cores;
	char model_name[64];
	char vendor_name[64];
	uint32_t base_mhz;
	uint32_t max_mhz;
	uint32_t ref_mhz;
};

int sys_sysconf(int num, long *rret) {
	struct meminfo mem;
	struct cpuinfo cpu;
	int ret, errno;
	long secs, nanos;

	switch (num) {
		case _SC_LINE_MAX:
			return 2048;
		case _SC_NGROUPS_MAX:
			return 0x10000;
		case _SC_CHILD_MAX:
			return 30;
		case _SC_NPROCESSORS_CONF:
			SYSCALL1(SYSCALL_GETCPUINFO, &cpu);
			if (ret == 0) {
				*rret = cpu.conf_cores;
				return 0;
			} else {
				return EFAULT;
			}
		case _SC_NPROCESSORS_ONLN:
			SYSCALL1(SYSCALL_GETCPUINFO, &cpu);
			if (ret == 0) {
				*rret = cpu.onln_cores;
				return 0;
			} else {
				return EFAULT;
			}
		case _SC_OPEN_MAX:
			*rret = 1024;
			return 0;
		case _SC_AVPHYS_PAGES:
			SYSCALL1(SYSCALL_MEMINFO, &mem);
			if (ret == 0) {
				*rret = mem.phys_free / getpagesize();
				return 0;
			} else {
				return EFAULT;
			}
		case _SC_PHYS_PAGES:
			SYSCALL1(SYSCALL_MEMINFO, &mem);
			if (ret == 0) {
				*rret = mem.phys_available / getpagesize();
				return 0;
			} else {
				return EFAULT;
			}
		case _SC_TOTAL_PAGES:
			SYSCALL1(SYSCALL_MEMINFO, &mem);
			if (ret == 0) {
				*rret = mem.phys_total / getpagesize();
				return 0;
			} else {
				return EFAULT;
			}
		case _SC_THREAD_STACK_MIN:
			*rret = 0x1000;
			return 0;
		case _SC_CLK_TCK:
			ret = sys_clock_getres(CLOCK_MONOTONIC, &secs, &nanos);
			if (ret == 0) {
				*rret = 1000000000 / nanos;
				return 0;
			} else {
				return ret;
			}
		default:
			return EINVAL;
	}
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	int ret, errno;
	(void)flags;

	switch (fsfdt) {
		case fsfd_target::fd: {
			SYSCALL5(SYSCALL_STAT, fd, "", 0, statbuf, AT_EMPTY_PATH);
			break;
		}
		case fsfd_target::path: {
			size_t len = strlen(path);
			SYSCALL5(SYSCALL_STAT, AT_FDCWD, path, len, statbuf, flags);
			break;
		}
		case fsfd_target::fd_path: {
			size_t len = strlen(path);
			SYSCALL5(SYSCALL_STAT, fd, path, len, statbuf, flags);
			break;
		}
		default: {
			__ensure(!"stat: Invalid fsfdt");
			__builtin_unreachable();
		}
	}

	return errno;
}

int sys_chmod(const char *pathname, mode_t mode) {
	int ret, errno;
	size_t len = strlen(pathname);
	SYSCALL5(SYSCALL_FCHMOD, AT_FDCWD, pathname, len, mode, 0);
	return errno;
}

int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
	int ret, errno;
	size_t len = strlen(pathname);
	SYSCALL5(SYSCALL_FCHMOD, fd, pathname, len, mode, flags);
	return errno;
}

int sys_fchmod(int fd, mode_t mode) {
	int ret, errno;
	SYSCALL5(SYSCALL_FCHMOD, fd, "", 0, mode, AT_EMPTY_PATH);
	return errno;
}

int sys_chown(const char *pathname, uid_t uid, gid_t gid) {
	int ret, errno;
	size_t len = strlen(pathname);
	SYSCALL6(SYSCALL_FCHOWN, AT_FDCWD, pathname, len, uid, gid, 0);
	return errno;
}

int sys_fchownat(int fd, const char *pathname, uid_t uid, gid_t gid, int flags) {
	int ret, errno;
	size_t len = strlen(pathname);
	SYSCALL6(SYSCALL_FCHOWN, fd, pathname, len, uid, gid, flags);
	return errno;
}

int sys_fchown(int fd, uid_t uid, gid_t gid) {
	int ret, errno;
	SYSCALL6(SYSCALL_FCHOWN, fd, "", 0, uid, gid, AT_EMPTY_PATH);
	return errno;
}

int sys_umask(mode_t mode, mode_t *old) {
	mode_t ret;
	int errno;
	SYSCALL1(SYSCALL_UMASK, mode);
	*old = (mode_t)ret;
	return errno;
}

int sys_fadvise(int fd, off_t offset, off_t length, int advice) {
	int ret, errno;
	SYSCALL4(SYSCALL_FADVISE, fd, offset, length, advice);
	return errno;
}

int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length) {
	ssize_t ret;
	int errno;
	size_t path_len = strlen(path);
	SYSCALL5(SYSCALL_READLINK, AT_FDCWD, path, path_len, buffer, max_size);
	if (ret == -1) {
		return errno;
	} else {
		*length = ret;
		return 0;
	}
}

int sys_rename(const char *path, const char *new_path) {
	int ret;
	int errno;
	size_t old_len = strlen(path);
	size_t new_len = strlen(new_path);
	SYSCALL7(SYSCALL_RENAME, AT_FDCWD, path, old_len, AT_FDCWD, new_path, new_len, 0);
	return errno;
}

int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
	int ret;
	int errno;
	size_t old_len = strlen(old_path);
	size_t new_len = strlen(new_path);
	SYSCALL7(SYSCALL_RENAME, olddirfd, old_path, old_len, newdirfd, new_path, new_len, 0);
	return errno;
}

int sys_mknodat(int dirfd, const char *path, int mode, int dev) {
	int ret;
	int errno;
	size_t len = strlen(path);
	SYSCALL5(SYSCALL_MAKENODE, dirfd, path, len, mode, dev);
	return errno;
}

int sys_mkfifoat(int dirfd, const char *path, mode_t mode) {
	return sys_mknodat(dirfd, path, S_IFIFO | mode, 0);
}

int sys_openpt(int oflags, int *fd) {
	int sfd, e;

	if (e = sys_openpty(fd, &sfd, NULL, NULL, NULL); e) {
		return e;
	}
	sys_close(sfd);

	int fdflags = 0;
	if (oflags & O_CLOEXEC) {
		fdflags |= FD_CLOEXEC;
	}
	if (oflags & O_CLOFORK) {
		fdflags |= FD_CLOFORK;
	}
	if (fdflags) {
		fcntl(*fd, F_SETFD, fdflags);
	}

	// We ignore non O_RDWR passed in oflags since that doesnt bond well with
	// the openpty interface.
	if (!(oflags & O_NOCTTY)) {
		ioctl(*fd, TIOCSCTTY);
	}

	return e;
}

int sys_unlockpt(int fd) {
	int unlock = 0;
	return sys_ioctl(fd, TIOCSPTLCK, &unlock, NULL);
}

int sys_symlink(const char *target, const char *link_path) {
	return sys_symlinkat(target, AT_FDCWD, link_path);
}

int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
	int ret;
	int errno;
	size_t target_len = strlen(target_path);
	size_t link_len = strlen(link_path);
	SYSCALL5(SYSCALL_SYMLINK, dirfd, target_path, target_len, link_path, link_len);
	return errno;
}

int sys_brk(void **out) {
	(void)out;
	return -1;
}

#define SC_LIST_MOUNTS 9
struct mountinfo {
	uint32_t type;
	uint32_t flags;
	char source[20];
	uint32_t source_length;
	char location[20];
	uint32_t location_length;
	uint64_t block_size;
	uint64_t fragment_size;
	uint64_t size_in_fragments;
	uint64_t free_blocks;
	uint64_t free_blocks_user;
	uint64_t inode_count;
	uint64_t free_inodes;
	uint64_t free_inodes_user;
	uint64_t max_filename;
};

#include <sys/mount.h>

int sys_fstatvfs(int fd, struct statvfs *out) {
	(void)fd;
	return sys_statvfs("/", out);
}

int sys_statvfs(const char *path, struct statvfs *out) {
	long ret, errno;
	struct mountinfo *buffer = (mountinfo *)malloc(5 * sizeof(struct mountinfo));
	SYSCALL2(SYSCALL_LISTMOUNTS, buffer, 5);
	if (errno) {
		free(buffer);
		return errno;
	} else if (ret > 5) {
		free(buffer);
		return 1;
	}

	bool best_len = 0;
	int  best_idx = 0;
	for (int i = 0; i < ret; i++) {
		if (!strncmp(path, buffer[i].location, buffer[i].location_length)) {
			if (buffer[i].location_length > best_len) {
				best_len = buffer[i].location_length;
				best_idx = i;
			}
		}
	}

	if (best_len == 0) {
		return EINVAL;
	}

	out->f_bsize  = buffer[best_idx].block_size;
	out->f_frsize = buffer[best_idx].fragment_size;
	out->f_blocks = buffer[best_idx].size_in_fragments;
	out->f_bfree  = buffer[best_idx].free_blocks;
	out->f_bavail = buffer[best_idx].free_blocks_user;
	out->f_files  = buffer[best_idx].inode_count;
	out->f_ffree  = buffer[best_idx].free_inodes;
	out->f_favail = buffer[best_idx].free_inodes_user;
	out->f_fsid = 0;
	out->f_flag = buffer[best_idx].flags;
	out->f_namemax = buffer[best_idx].max_filename;

	switch (buffer[best_idx].type) {
		case MNT_EXT: strcpy(out->f_basetype, "ext");   break;
		case MNT_FAT: strcpy(out->f_basetype, "fat");   break;
		default:      strcpy(out->f_basetype, "devfs"); break;
	}

	return 0;
}

int sys_shmat(void **seg_start, int shmid, const void *shmaddr, int shmflg) {
	void *ret;
	int errno;
	SYSCALL3(SYSCALL_SHMAT, shmid, shmaddr, shmflg);
	*seg_start = ret;
	return errno;
}

int sys_shmctl(int *idx, int shmid, int cmd, struct shmid_ds *buf) {
	int ret, errno;
	SYSCALL3(SYSCALL_SHMCTL, shmid, cmd, buf);
	*idx = ret;
	return errno;
}

int sys_shmdt(const void *shmaddr) {
	int ret, errno;
	SYSCALL1(SYSCALL_SHMDT, shmaddr);
	return errno;
}

int sys_shmget(int *shm_id, key_t key, size_t size, int shmflg) {
	int ret, errno;
	SYSCALL3(SYSCALL_SHMGET, key, size, shmflg);
	*shm_id = ret;
	return errno;
}

int sys_getloadavg(double *samples) {
	int ret, errno;
	int samples2[3];
	SYSCALL2(SYSCALL_LOADAVG, samples2, 3);
	if (ret < 0) {
		return errno;
	}
	for (int i = 0; i < 3; i++) {
		samples[i] = samples2[i] / 100.0;
	}
	return 0;
}

int sys_openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win) {
	int ret;
	int fds[2];
	SYSCALL1(SYSCALL_OPENPTY, fds);
	if (errno) {
		return errno;
	}
	*mfd = fds[0];
	*sfd = fds[1];

	if (name != NULL) {
		name = ttyname(*mfd);
		if (!name) {
			return errno;
		}
	}

	if (ios == NULL) {
		struct termios termios = {};
		termios.c_iflag = BRKINT | IGNPAR | ICRNL | IXON | IMAXBEL;
		termios.c_oflag = OPOST | ONLCR;
		termios.c_cflag = CS8 | CREAD;
		termios.c_lflag = ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE;
		termios.c_cc[VINTR] = CTRL('C');
		termios.c_cc[VERASE] = 127; // Delete.
		termios.c_cc[VEOF] = CTRL('D');
		termios.c_cc[VSUSP] = CTRL('Z');
		termios.ibaud = 38400;
		termios.obaud = 38400;
		ret = tcsetattr(*mfd, TCSANOW, &termios);
	} else {
		ret = tcsetattr(*mfd, TCSANOW, ios);
	}
	if (ret) {
		return errno;
	}

	if (win == NULL) {
		struct winsize win_size = {
			.ws_row = 24,
			.ws_col = 80,
			.ws_xpixel = 24 * 16,
			.ws_ypixel = 80 * 16
		};
		ret = ioctl(*mfd, TIOCSWINSZ, &win_size);
	} else {
		ret = ioctl(*mfd, TIOCSWINSZ, win);
	}

	if (ret) {
		return errno;
	}

	return 0;
}

#endif
} // namespace mlibc
