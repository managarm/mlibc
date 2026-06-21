
#include <abi-bits/fcntl.h>
#include <abi-bits/getid.h>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>

// ANCHOR: stub
#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})
// ANCHOR_END: stub

namespace mlibc {

int Sysdeps<Write>::operator()(int fd, void const *buf, size_t size, ssize_t *ret) {
	*ret = __syscall_fs_write(fd, (__mlibc_uint8 const *)buf, size);
	return *ret >= 0 ? 0 : -*ret;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *ret) {
	auto res = __syscall_fs_seek(fd, offset, whence);
	*ret = res;
	return res < 0 ? -res : 0;
}

void Sysdeps<Exit>::operator()(int status) {
	__syscall_proc_exit(status);
	__builtin_unreachable();
}

int Sysdeps<Close>::operator()(int fd) { return -__syscall_fs_close(fd); }

int Sysdeps<FutexWake>::operator()(int *, bool) {
	// TODO: Proper impl would make a syscall to notify.
	return 0;
}

int Sysdeps<FutexWait>::operator()(int *, int, timespec const *) {
	// TODO: Proper impl
	return 0;
}

int Sysdeps<ReadEntries>::operator()(int fd, void *buffer, size_t max_size, size_t *bytes_read) {
	auto res = __syscall_fs_getdents(fd, (__mlibc_uint8 *)buffer, max_size);
	*bytes_read = res;
	return res < 0 ? -res : 0;
}

int Sysdeps<Read>::operator()(int fd, void *read_buf, unsigned long read_len, long *ret) {
	auto res = __syscall_fs_read(fd, (__mlibc_uint8 *)read_buf, read_len);
	*ret = res;
	return res < 0 ? -res : 0;
}

int Sysdeps<Open>::operator()(const char *path, int oflags, unsigned int mode, int *ret) {
	(void)mode;
	// TODO: Mode currently ignored.
	auto res = __syscall_fs_open(AT_FDCWD, path, oflags);
	*ret = res;
	return res < 0 ? -res : 0;
}

int Sysdeps<ClockGet>::operator()(int clkid, time_t *secs, long *nanos) {
	struct timespec ts;
	auto res = __syscall_time_gettime(clkid, &ts);
	*secs = ts.tv_sec;
	*nanos = ts.tv_nsec;
	return -res;
}

int Sysdeps<Fork>::operator()(pid_t *child) {
	pid_t res = __syscall_proc_fork();
	if (res < 0) {
		return -res;
	} else {
		*child = res;
		return 0;
	}
}

int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	return -__syscall_proc_exec(path, argv, envp);
}

pid_t Sysdeps<GetPid>::operator()() { return __syscall_proc_getid(_GETID_PID); }

int Sysdeps<Sigaction>::operator()(
    int signum,
    const struct sigaction *__restrict newhandler,
    struct sigaction *__restrict oldhandler
) {
	if (newhandler) {
		struct sigaction tmp = *newhandler;
		tmp.sa_restorer = (void (*)())__syscall_proc_sigret;
		return -__syscall_proc_sigaction(signum, &tmp, oldhandler);
	} else {
		return -__syscall_proc_sigaction(signum, nullptr, oldhandler);
	}
}

int Sysdeps<Kill>::operator()(pid_t pid, int signo) { return -__syscall_proc_kill(pid, signo); }

int
Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	if (ru) {
		memset(ru, 0, sizeof(struct rusage));
	}
	auto res = __syscall_proc_waitpid(pid, status, flags | WEXITED);
	if (ret_pid) {
		*ret_pid = res;
	}
	return res < 0 ? -res : 0;
}

int Sysdeps<Rename>::operator()(const char *old_path, const char *new_path) {
	return -__syscall_fs_rename(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int Sysdeps<Unlinkat>::operator()(int fd, const char *path, int flags) {
	if (flags & AT_REMOVEDIR) {
		return -__syscall_fs_rmdir(fd, path);
	} else {
		return -__syscall_fs_unlink(fd, path);
	}
}

int Sysdeps<Rmdir>::operator()(const char *path) { return -__syscall_fs_rmdir(AT_FDCWD, path); }

int Sysdeps<Sigprocmask>::operator()(
    int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve
) {
	return -__syscall_thread_sigmask(how, set, retrieve);
}

int Sysdeps<Isatty>::operator()(int fd) { return -__syscall_fs_isatty(fd); }

} // namespace mlibc
