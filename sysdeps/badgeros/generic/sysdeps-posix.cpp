
#include <abi-bits/getid.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/syscall.h>

// ANCHOR: stub
#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})
// ANCHOR_END: stub

namespace mlibc {

int Sysdeps<OpenDir>::operator()(const char *path, int *ret) {
	auto res = __syscall_fs_open(AT_FDCWD, path, O_DIRECTORY);
	*ret = res;
	return res < 0 ? -res : 0;
}

gid_t Sysdeps<GetGid>::operator()() { return __syscall_proc_getid(_GETID_GID); }
gid_t Sysdeps<GetEgid>::operator()() { return __syscall_proc_getid(_GETID_EGID); }
uid_t Sysdeps<GetUid>::operator()() { return __syscall_proc_getid(_GETID_UID); }
uid_t Sysdeps<GetEuid>::operator()() { return __syscall_proc_getid(_GETID_EUID); }
pid_t Sysdeps<GetTid>::operator()() { return __syscall_proc_getid(_GETID_TID); }
pid_t Sysdeps<GetPpid>::operator()() { return __syscall_proc_getid(_GETID_PPID); }
int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
	*pgid = 1;
	return 0;
}
int Sysdeps<GetSid>::operator()(pid_t pid, pid_t *sid) {
	*sid = 1;
	return 0;
}

int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
	(void)mode;
	// TODO: Mode currently ignored.
	return -__syscall_fs_mkdir(AT_FDCWD, path);
}

int Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode) {
	(void)mode;
	// TODO: Mode currently ignored.
	return -__syscall_fs_mkdir(dirfd, path);
}

int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
	return -__syscall_fs_link(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int Sysdeps<Linkat>::operator()(
    int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags
) {
	return -__syscall_fs_link(olddirfd, old_path, newdirfd, new_path, flags);
}

int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
	return -__syscall_fs_symlink(target_path, AT_FDCWD, link_path);
}

int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
	return -__syscall_fs_symlink(target_path, dirfd, link_path);
}

int Sysdeps<Renameat>::operator()(
    int olddirfd, const char *old_path, int newdirfd, const char *new_path
) {
	return -__syscall_fs_rename(olddirfd, old_path, newdirfd, new_path, 0);
}

int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
	auto res = __syscall_fs_dup(fd, flags, -1);
	*newfd = res;
	return res < 0 ? -res : 0;
}

int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
	auto res = __syscall_fs_dup(fd, flags, newfd);
	return res < 0 ? -res : 0;
}

int Sysdeps<ThreadSigmask>::operator()(
    int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve
) {
	return -__syscall_thread_sigmask(how, set, retrieve);
}

int Sysdeps<Uname>::operator()(struct utsname *buf) { return -__syscall_sys_uname(buf); }

int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr) {
	return -__syscall_fs_tcgetattr(fd, attr);
}

int Sysdeps<Tcsetattr>::operator()(int fd, int act, const struct termios *attr) {
	(void)act;
	return -__syscall_fs_tcsetattr(fd, attr);
}

int Sysdeps<GetCwd>::operator()(char *buffer, size_t size) {
	return -__syscall_fs_getcwd(buffer, size);
}

int Sysdeps<Chdir>::operator()(const char *path) { return -__syscall_fs_chdir(-1, path); }

int Sysdeps<Fchdir>::operator()(int fd) { return -__syscall_fs_chdir(fd, nullptr); }

int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result) {
	switch (request) {
		case F_DUPFD: {
			int minfd = va_arg(args, int);
			auto res = __syscall_fs_dup(fd, DUP2_FCNTL, minfd);
			*result = res;
			return res < 0 ? -res : 0;
		}
		case F_DUPFD_CLOEXEC: {
			int minfd = va_arg(args, int);
			auto res = __syscall_fs_dup(fd, DUP2_FCNTL | O_CLOEXEC, minfd);
			*result = res;
			return res < 0 ? -res : 0;
		}
		case F_GETFD: {
			auto res = __syscall_fs_getfd(fd);
			*result = res;
			return res < 0 ? -res : 0;
		}
		case F_SETFD: {
			return -__syscall_fs_setfd(fd, va_arg(args, int));
		}
		case F_GETFL: {
			auto res = __syscall_fs_getfl(fd);
			*result = res;
			return res < 0 ? -res : 0;
		}
		case F_SETFL: {
			return -__syscall_fs_setfl(fd, va_arg(args, int));
		}
		default:
			return EINVAL;
	}
}

} // namespace mlibc
