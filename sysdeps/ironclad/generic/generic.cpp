#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/ironclad_devices.h>
#include <sched.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/file.h>

namespace mlibc {

void sys_libc_log(const char *message) {
	ssize_t unused;
	char new_line = '\n';
	sys_write(2, message, strlen(message), &unused);
	sys_write(2, &new_line, 1, &unused);
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
	int ret, errno;
	SYSCALL2(SYSCALL_ARCH_PRCTL, 1, pointer);
	return errno;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	return sys_openat(AT_FDCWD, path, flags, mode, fd);
}

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	int ret, errno;

	int path_len = strlen (path);
	SYSCALL4(SYSCALL_OPEN, dirfd, path, path_len, flags);
	if (ret != -1 && (flags & O_EXCL)) {
		 SYSCALL1(SYSCALL_CLOSE, ret);
		 return EEXIST;
	}

	if (ret == -1 && (flags & O_CREAT)) {
		 SYSCALL5(SYSCALL_MAKENODE, AT_FDCWD, path, path_len, mode, 0);
		 if (ret == -1) {
			  return errno;
		 }
		 SYSCALL4(SYSCALL_OPEN,	AT_FDCWD, path, path_len, flags);
	} else if (ret != -1 && (flags & O_TRUNC)) {
		// If the file cannot be truncated, dont sweat it, some software
		// depends on some things being truncate-able that ironclad does not
		// allow. For example, some devices.
		sys_ftruncate(ret, 0);
	} else if (ret != -1 && (flags & O_DIRECTORY)) {
		struct stat st;
		sys_stat(fsfd_target::fd, ret, NULL, 0, &st);
		if (!S_ISDIR (st.st_mode)) {
			ret	= -1;
			errno = ENOTDIR;
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
	SYSCALL3(SYSCALL_READ, fd, buf, count);
	*bytes_read = ret;
	return errno;
}

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	ssize_t ret;
	int errno;
	SYSCALL3(SYSCALL_WRITE, fd, buf, count);
	*bytes_written = ret;
	return errno;
}

int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	ssize_t ret;
	int errno;
	SYSCALL4(SYSCALL_PREAD, fd, buf, n, off);
	*bytes_read = ret;
	return errno;
}

int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
	ssize_t ret;
	int errno;
	SYSCALL4(SYSCALL_WRITE, fd, buf, n, off);
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

int sys_flock(int fd, int options) {
	struct flock lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = (off_t)((uint64_t)-1);
	lock.l_pid = sys_getpid();

	switch (options) {
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

	int ret, errno;
	SYSCALL3(SYSCALL_FCNTL, fd, F_SETLK, &lock);
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
	return errno;
}

int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
	(void)fd;
	(void)size;
	if (layer == SOL_SOCKET && number == SO_PEERCRED) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_PEERCRED is unimplemented" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_SNDBUF is unimplemented" << frg::endlog;
		*(int *)buffer = 4096;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_TYPE) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_TYPE is unimplemented, hardcoding SOCK_STREAM" << frg::endlog;
		*(int *)buffer = SOCK_STREAM;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_ERROR) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_ERROR is unimplemented, hardcoding 0" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "mlibc: getsockopt() call with SOL_SOCKET and SO_KEEPALIVE is unimplemented, hardcoding 0" << frg::endlog;
		*(int *)buffer = 0;
		return 0;
	} else {
		mlibc::panicLogger() << "mlibc: Unexpected getsockopt() call, layer: " << layer << " number: " << number << frg::endlog;
	}

	return 0;
}

int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
	(void)fd;
	(void)buffer;
	(void)size;
	if (layer == SOL_SOCKET && number == SO_PASSCRED) {
		mlibc::infoLogger() << "mlibc: setsockopt(SO_PASSCRED) is not implemented correctly" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_ATTACH_FILTER) {
		mlibc::infoLogger() << "mlibc: setsockopt(SO_ATTACH_FILTER) is not implemented correctly" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_RCVBUFFORCE) {
		mlibc::infoLogger() << "mlibc: setsockopt(SO_RCVBUFFORCE) is not implemented correctly" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_SNDBUF) {
		mlibc::infoLogger() << "mlibc: setsockopt() call with SOL_SOCKET and SO_SNDBUF is unimplemented" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_KEEPALIVE) {
		mlibc::infoLogger() << "mlibc: setsockopt() call with SOL_SOCKET and SO_KEEPALIVE is unimplemented" << frg::endlog;
		return 0;
	} else if (layer == SOL_SOCKET && number == SO_REUSEADDR) {
		mlibc::infoLogger() << "mlibc: setsockopt() call with SOL_SOCKET and SO_REUSEADDR is unimplemented" << frg::endlog;
		return 0;
	} else if (layer == AF_NETLINK && number == SO_ACCEPTCONN) {
		mlibc::infoLogger() << "mlibc: setsockopt() call with AF_NETLINK and SO_ACCEPTCONN is unimplemented" << frg::endlog;
		return 0;
	} else {
		mlibc::panicLogger() << "mlibc: Unexpected setsockopt() call, layer: " << layer << " number: " << number << frg::endlog;
	}

	 return 0;
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

pid_t sys_getpid() {
	pid_t ret;
	int errno;
	SYSCALL0(SYSCALL_GETPID);
	return ret;
}

pid_t sys_getppid() {
	pid_t ret;
	int errno;
	SYSCALL0(SYSCALL_GETPPID);
	return ret;
}

int sys_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	return 0;
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
	return 0;
}

int sys_isatty(int fd) {
	struct termios t;
	if (sys_tcgetattr(fd, &t) == 0) {
		return 0;
	} else {
		return ENOTTY;
	}
}

gid_t sys_getgid() {
	// FIXME: Stub needed by mlibc.
	return 0;
}

gid_t sys_getegid() {
	// FIXME: Stub needed by mlibc.
	return 0;
}

int sys_getpgid(pid_t pid, pid_t *pgid) {
	(void)pid;
	// FIXME: Stub needed by mlibc.
	*pgid = 0;
	return 0;
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

	SYSCALL6(SYSCALL_CLONE, 0, 0, 0, 0, 0, 1);

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
	SYSCALL1(SYSCALL_GETRLIMIT, resource);
	limit->rlim_cur = ret;
	limit->rlim_max = ret;
	return errno;
}

int sys_setrlimit(int resource, const struct rlimit *limit) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETRLIMIT, resource, limit->rlim_cur);
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

	SYSCALL3(SYSCALL_SYSCONF, 10, buf, sizeof(struct utsname));

	if (ret == -1) {
		return errno;
	}

	return 0;
}



int sys_setpgid(pid_t pid, pid_t pgid) {
	(void)pid;
	(void)pgid;
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
	return 0;
}

int sys_ttyname(int, char *, size_t) {
	return ENOSYS;
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

int sys_kill(int pid, int sig) {
	int ret, errno;
	if (sig == SIGKILL) {
		SYSCALL1(SYSCALL_ACTUALLY_KILL, pid);
	} else {
		SYSCALL2(SYSCALL_SEND_SIGNAL, pid, sig);
	}

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
	return errno;
}

int sys_dup2(int fd, int flags, int newfd) {
	if (sys_close(newfd) != 0) {
		 return EBADFD;
	}

	int ret, errno;
	if (flags & O_CLOEXEC) {
		SYSCALL3(SYSCALL_FCNTL, fd, F_DUPFD_CLOEXEC, newfd);
	} else {
		SYSCALL3(SYSCALL_FCNTL, fd, F_DUPFD, newfd);
	}

	if (ret != -1 && ret != newfd) {
		return EBADFD;
	} else {
		return errno;
	}
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
	struct futex_item item = {.addr = (uint64_t)pointer, .expected = expected, .flags = 0};
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
	SYSCALL2(SYSCALL_GETRANDOM, buffer, length);
	return errno;
}

int sys_mkdir(const char *path, mode_t mode) {
	return sys_mkdirat(AT_FDCWD, path, mode);
}

int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	int ret, errno;
	size_t path_len = strlen (path);
	SYSCALL5(SYSCALL_MAKENODE, dirfd, path, path_len, S_IFDIR | mode, 0);
	return errno;
}

int sys_rmdir(const char* path){
	int ret, errno;
	size_t path_len = strlen (path);
	SYSCALL3(SYSCALL_UNLINK, AT_FDCWD, path, path_len);
	return errno;
}

int sys_unlinkat(int fd, const char *path, int flags) {
	int ret, errno;
	size_t path_len = strlen (path);
	SYSCALL3(SYSCALL_UNLINK, fd, path, path_len);
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
	int ret, errno;
	SYSCALL3(SYSCALL_SOCKET, domain, type, protocol);
	if (ret != -1) {
		*fd = ret;
		return 0;
	} else {
		return errno;
	}
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
	if (uid == 0) {
		 SYSCALL2(SYSCALL_SETUIDS, uid, uid);
	} else {
		 SYSCALL2(SYSCALL_SETUIDS, uid, ((uint64_t)-1));
	}
	return ret;
}

int sys_seteuid(uid_t euid) {
	int ret, errno;
	SYSCALL2(SYSCALL_SETUIDS, ((uint64_t)-1), euid);
	return ret;
}

#ifndef MLIBC_BUILDING_RTDL

extern "C" void __mlibc_thread_entry();

int sys_clone(void *tcb, pid_t *tid_out, void *stack) {
	 int ret, errno;
	 SYSCALL6(SYSCALL_CLONE, (uintptr_t)__mlibc_thread_entry, 0, stack, 0b10, tcb, 1);

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

int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length) {
	if (hdr->msg_control != NULL) {
		mlibc::infoLogger() << "mlibc: recv() msg_control not supported!" << frg::endlog;
		return EINVAL;
	}

	int ret;
	size_t count = 0;
	int errno;

	for (int i = 0; i < hdr->msg_iovlen; i++) {
		SYSCALL6(SYSCALL_RECVFROM, fd, hdr->msg_iov->iov_base, hdr->msg_iov->iov_len,
					hdr->msg_flags, hdr->msg_name, hdr->msg_namelen);
		if (ret == -1) {
			return errno;
		}
		count += ret;
	}

	*length = count;
	return 0;
}

int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length) {
	if (hdr->msg_control != NULL) {
		mlibc::infoLogger() << "mlibc: recv() msg_control not supported!" << frg::endlog;
		return EINVAL;
	}

	int ret;
	size_t count = 0;
	int errno;

	for (int i = 0; i < hdr->msg_iovlen; i++) {
		SYSCALL6(SYSCALL_SENDTO, fd, hdr->msg_iov->iov_base, hdr->msg_iov->iov_len,
					hdr->msg_flags, hdr->msg_name, hdr->msg_namelen);
		if (ret == -1) {
			return errno;
		}
		count += ret;
	}

	*length = count;
	return 0;
}


int sys_ppoll(struct pollfd *fds, int nfds, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
	// XXX: Ironclad has no sigprogmask so this is basically a weird ppoll poll
	// chimeral abomination.
	int ret, errno;
	if (timeout == NULL) {
		 struct timespec t = {.tv_sec = (time_t)-1, .tv_nsec = (time_t)-1};
		 SYSCALL3(SYSCALL_POLL, fds, nfds, &t);
	} else {
		 SYSCALL3(SYSCALL_POLL, fds, nfds, timeout);
	}
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
	return sys_ppoll(fds, count, timeout < 0 ? NULL : &ts, NULL, num_events);
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

int sys_sysconf(int num, long *rret) {
	long ret, errno;

	// Translate the number for the POSIX compat of the sysconf.
	int translated;
	switch (num) {
		case _SC_PAGE_SIZE:		  translated = 1;  break;
		case _SC_OPEN_MAX:			translated = 2;  break;
		case _SC_HOST_NAME_MAX:	 translated = 3;  break;
		case _SC_NPROCESSORS_ONLN: translated = 6;  break;
		case _SC_PHYS_PAGES:		 translated = 7;  break;
		case _SC_CHILD_MAX:		  translated = 11; break;

		// These are values that mlibc technically has to provide itself, but
		// I personally dont like how ugly the hardcoded warnings look, so I just
		// harcode them myself :)
		case _SC_LINE_MAX: *rret = 4096; return 0;

		// Default is for mlibc to handle it.
		default: return EINVAL;
	}

	SYSCALL3(SYSCALL_SYSCONF, translated, 0, 0);
	*rret = ret;
	return errno;
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

int sys_mknodat(int dirfd, const char *path, mode_t mode, dev_t dev) {
	int ret;
	int errno;
	size_t len = strlen(path);
	SYSCALL5(SYSCALL_MAKENODE, dirfd, path, len, mode, dev);
	return errno;
}

#endif
} // namespace mlibc
