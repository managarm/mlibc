
#include <errno.h>
#include <bits/ensure.h>
#include <sys/stat.h>

#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

int chmod(const char *, mode_t) {
	mlibc::infoLogger() << "\e[31mmlibc: chmod() is not implemented correctly\e[39m"
			<< frg::endlog;
	return 0;
}

int fchmod(int, mode_t) {
	mlibc::infoLogger() << "\e[31mmlibc: fchmod() is not implemented correctly\e[39m"
			<< frg::endlog;
	return 0;
}

int fchmodat(int, const char *, mode_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fstatat(int dirfd, const char *path, struct stat *result, int flags) {
	if(!mlibc::sys_stat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_stat(mlibc::fsfd_target::fd_path, dirfd, path, flags, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}
int futimens(int fd, const struct timespec times[2]) {
	mlibc::infoLogger() << "\e[31mmlibc: futimens() is not implemented correctly\e[39m"
			<< frg::endlog;
	return 0;
}
int mkdir(const char *path, mode_t) {
	mlibc::infoLogger() << "\e[31mmlibc: mkdir() ignores its mode\e[39m" << frg::endlog;
	if(!mlibc::sys_mkdir) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_mkdir(path); e) {
		errno = e;
		return -1;
	}
	return 0;
}
int mkdirat(int, const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mkfifo(const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mkfifoat(int, const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mknod(const char *, mode_t, dev_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mknodat(int, const char *, mode_t, dev_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
mode_t umask(mode_t) {
	mlibc::infoLogger() << "\e[31mmlibc: umask() is a no-op and always returns 0\e[39m"
			<< frg::endlog;
	return 0;
}
int utimensat(int, const char *, const struct timespec times[2], int) {
	mlibc::infoLogger() << "\e[31mmlibc: utimensat() is not implemented correctly\e[39m"
			<< frg::endlog;
	return 0;
}


int stat(const char *path, struct stat *result) {
	if(!mlibc::sys_stat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_stat(mlibc::fsfd_target::path, -1, path, 0, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int lstat(const char *path, struct stat *result) {
	if(!mlibc::sys_stat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_stat(mlibc::fsfd_target::path,
			-1, path, AT_SYMLINK_NOFOLLOW, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fstat(int fd, struct stat *result) {
	if(!mlibc::sys_stat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_stat(mlibc::fsfd_target::fd, fd, nullptr, 0, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

