
#include <errno.h>
#include <bits/ensure.h>
#include <sys/stat.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int chmod(const char *pathname, mode_t mode) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_chmod, -1);
	if(int e = mlibc::sys_chmod(pathname, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchmod(int fd, mode_t mode) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fchmod, -1);
	if(int e = mlibc::sys_fchmod(fd, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fchmodat, -1);
	if(int e = mlibc::sys_fchmodat(dirfd, pathname, mode, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fstatat(int dirfd, const char *path, struct stat *result, int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_stat, -1);
	if(int e = mlibc::sys_stat(mlibc::fsfd_target::fd_path, dirfd, path, flags, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int futimens(int fd, const struct timespec times[2]) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_utimensat, -1);

	if (int e = mlibc::sys_utimensat(fd, nullptr, times, 0); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int mkdir(const char *path, mode_t mode) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_mkdir, -1);
	if(int e = mlibc::sys_mkdir(path, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mkdirat(int dirfd, const char *path, mode_t mode) {
	mlibc::infoLogger() << "\e[31mmlibc: mkdirat() ignores its mode\e[39m" << frg::endlog;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_mkdirat, -1);
	if(int e = mlibc::sys_mkdirat(dirfd, path, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mkfifo(const char *path, mode_t mode) {
	return mkfifoat(AT_FDCWD, path, mode);
}

int mkfifoat(int dirfd, const char *path, mode_t mode) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_mkfifoat, -1);
	if (int e = mlibc::sys_mkfifoat(dirfd, path, mode); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int mknod(const char *path, mode_t mode, dev_t dev) {
	return mknodat(AT_FDCWD, path, mode, dev);
}

int mknodat(int dirfd, const char *path, mode_t mode, dev_t dev) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_mknodat, -1);
	if (int e = mlibc::sys_mknodat(dirfd, path, mode, dev); e) {
		errno = e;
		return -1;
	}

	return 0;
}

mode_t umask(mode_t mode) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_umask, -1);
	mode_t old;
	if (int e = mlibc::sys_umask(mode, &old); e) {
		errno = e;
		return -1;
	}
	return old;
}

int utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	if(pathname == nullptr) {
		errno = EINVAL;
		return -1;
	}

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_utimensat, -1);
	if (int e = mlibc::sys_utimensat(dirfd, pathname, times, flags); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int stat(const char *path, struct stat *result) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_stat, -1);
	if(int e = mlibc::sys_stat(mlibc::fsfd_target::path, -1, path, 0, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int lstat(const char *path, struct stat *result) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_stat, -1);
	if(int e = mlibc::sys_stat(mlibc::fsfd_target::path,
			-1, path, AT_SYMLINK_NOFOLLOW, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

[[gnu::alias("lstat")]] int lstat64(const char *path, struct stat64 *result);

int fstat(int fd, struct stat *result) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_stat, -1);
	if(int e = mlibc::sys_stat(mlibc::fsfd_target::fd, fd, "", 0, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

[[gnu::alias("fstat")]] int fstat64(int fd, struct stat64 *result);
