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

namespace mlibc {

void sys_libc_log(const char *message) {
	/*
	ssize_t unused;
	char new_line = '\n';
	sys_write(1, message, strlen(message), &unused);
	sys_write(1, &new_line, 1, &unused);
	*/
	(void)message;
	return;
}

void sys_libc_panic() {
	ssize_t unused;
	char const *message = "mlibc panicked unrecoverably\n";
	sys_write(1, message, strlen(message), &unused);
	sys_exit(1);
}

void sys_exit(int status) {
	int ret, errno;
	SYSCALL1(SYSCALL_EXIT, status);
	__builtin_unreachable();
}

int sys_tcb_set(void *pointer) {
	int ret, errno;
	SYSCALL1(SYSCALL_SET_TCB, pointer);
	return errno;
}

int sys_open(const char *path, int flags, mode_t mode, int *fd) {
	int ret, errno;
	(void)mode;

	SYSCALL2(SYSCALL_OPEN, path, flags);
	*fd = ret;
	return errno;
}

int sys_open_dir(const char *path, int *handle) {
	return sys_open(path, O_RDONLY, 0660, handle);
}

int sys_close(int fd) {
	int ret, errno;
	SYSCALL1(SYSCALL_CLOSE, fd);
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

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	ssize_t ret;
	int errno;
	SYSCALL3(SYSCALL_SEEK, fd, offset, whence);
	*new_offset = ret;
	return errno;
}

int sys_anon_allocate(size_t size, void **pointer) {
	return sys_vm_map(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON, 0, 0, pointer);
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
	(void)signum;
	(void)act;
	(void)oldact;
	mlibc::infoLogger() << "sigaction() is a stub" << frg::endlog;
	return 0;
}

int sys_fcntl(int fd, int request, va_list args, int *result) {
	(void)fd;
	(void)request;
	(void)args;
	(void)result;
	mlibc::infoLogger() << "fcntl() is a stub!" << frg::endlog;
	return 0;
}

int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	(void)how;
	(void)set;
	(void)retrieve;
	mlibc::infoLogger() << "sigprocmask() is a stub!" << frg::endlog;
	return ENOSYS;
}

int sys_ttyname(int fd, char *buf, size_t size) {
	(void)fd;
	(void)buf;
	(void)size;
	mlibc::infoLogger() << "ttyname() is a stub!" << frg::endlog;
	return ENOSYS;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	(void)pid;
	(void)pgid;
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
	return 0;
}

int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set,
	const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
	(void)num_fds;
	(void)read_set;
	(void)write_set;
	(void)except_set;
	(void)timeout;
	(void)sigmask;
	(void)num_events;
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return 0;
}

int sys_isatty(int fd) {
	(void)fd;
	mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return 0;
}

uid_t sys_getuid() {
	mlibc::infoLogger() << "getuid() is a stub" << frg::endlog;
	return 0;
}

uid_t sys_geteuid() {
	mlibc::infoLogger() << "geteuid() is a stub" << frg::endlog;
	return 0;
}

gid_t sys_getgid() {
	mlibc::infoLogger() << "getgid() is a stub" << frg::endlog;
	return 0;
}

gid_t sys_getegid() {
	mlibc::infoLogger() << "getegid() is a stub" << frg::endlog;
	return 0;
}

int sys_getpgid(pid_t pid, pid_t *pgid) {
	(void)pid;
	mlibc::infoLogger() << "getpgid() is a stub" << frg::endlog;
	*pgid = 0;
	return 0;
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	(void)clock;
	(void)secs;
	(void)nanos;
	mlibc::infoLogger() << "clock_get() is a stub" << frg::endlog;
	return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	int ret, errno;

	SYSCALL3(SYSCALL_EXEC, path, argv, envp);

	if (ret == -1) {
		return errno;
    }

	return 0;
}

int sys_fork(pid_t *child) {
    pid_t ret;
    int errno;

    SYSCALL0(SYSCALL_FORK);

    if (ret == -1) {
        return errno;
    }

    *child = ret;
    return 0;
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
	return 0;
}

int sys_uname(struct utsname *buf) {
	int ret, errno;

	SYSCALL1(SYSCALL_UNAME, buf);

	if (ret == -1) {
		return errno;
	}

	return 0;
}

int sys_sethostname(const char *buff, size_t size) {
	int ret, errno;

	SYSCALL2(SYSCALL_SETHOSTNAME, buff, size);

	if (ret == -1) {
		return errno;
	}

	return 0;
}

int sys_getcwd(const char *buff, size_t size) {
	char* ret;
	int errno;

	SYSCALL2(SYSCALL_GETCWD, buff, size);

	if (ret == NULL) {
		return errno;
	}

	return 0;
}

int sys_chdir(const char *buff) {
	int ret, errno;

	SYSCALL1(SYSCALL_CHDIR, buff);

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

int sys_sched_yield(void) {
	int ret, errno;

	SYSCALL0(SYSCALL_SCHED_YIELD);

	if (ret == -1) {
		return errno;
	}

	return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
	(void)flags;

	int ret, errno;

	SYSCALL1(SYSCALL_DUP, fd);

	if (ret == -1) {
        return errno;
	}
	*newfd = ret;
	return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
	int ret, errno;

	if (flags) {
		SYSCALL3(SYSCALL_DUP3, fd, newfd, flags);
	} else {
		SYSCALL2(SYSCALL_DUP2, fd, newfd);
	}

	if (ret == -1) {
        return errno;
	} else {
		return 0;
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

	SYSCALL2(SYSCALL_ACCESS, path, mode);

	if (ret == -1) {
        return errno;
	} else {
        return 0;
   }
}

int sys_getpriority(int which, id_t who, int *value) {
	int ret, errno;

	SYSCALL2(SYSCALL_GETPRIORITY, which, who);

	if (ret == -1 && errno != 0) {
        return errno;
	} else {
			*value = ret;
        return 0;
   }
}

int sys_setpriority(int which, id_t who, int prio) {
	int ret, errno;

	SYSCALL3(SYSCALL_SETPRIORITY, which, who, prio);

	if (ret == -1) {
        return errno;
	} else {
        return 0;
   }
}

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	(void)pointer;
	(void)expected;
	(void)time;
	__ensure(!"sys_futex is a stub!");
	__builtin_unreachable();
}

int sys_futex_wake(int *pointer) {
	(void)pointer;
	__ensure(!"sys_futex is a stub!");
	__builtin_unreachable();
}

#ifndef MLIBC_BUILDING_RTDL

int sys_gethostname(char *buffer, size_t bufsize) {
	struct utsname buf;
	if (uname(&buf)) {
		return -1;
	}

	strncpy(buffer, buf.nodename, bufsize);
	return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	int ret, errno;
	(void)flags;

	switch (fsfdt) {
		case fsfd_target::fd: {
			// mlibc wants us to do a fstat.
			SYSCALL2(SYSCALL_FSTAT, fd, statbuf);
			return errno;
		}
		case fsfd_target::path: {
			// mlibc wants us to do a lstat.
			SYSCALL2(SYSCALL_LSTAT, path, statbuf);
			return errno;
		}
		case fsfd_target::fd_path: {
			// mlibc wants us to do an fstatat
			__ensure(!"stat: Invalid fstatat");
			__builtin_unreachable();
		}
		default: {
			__ensure(!"stat: Invalid fsfdt");
			__builtin_unreachable();
		}
	}

	__builtin_unreachable();
}

#endif
} // namespace mlibc
