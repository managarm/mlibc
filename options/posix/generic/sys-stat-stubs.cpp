
#include <errno.h>
#include <bits/ensure.h>
#include <sys/stat.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int chmod(const char *pathname, mode_t mode) {
	if(!mlibc::sys_chmod) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_chmod(pathname, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchmod(int fd, mode_t mode) {
	if(!mlibc::sys_fchmod) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_fchmod(fd, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags) {
	if(!mlibc::sys_fchmodat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_fchmodat(dirfd, pathname, mode, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
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
	if (!mlibc::sys_utimensat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	if (int e = mlibc::sys_utimensat(fd, nullptr, times, 0); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int mkdir(const char *path, mode_t mode) {
	if(!mlibc::sys_mkdir) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_mkdir(path, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mkdirat(int dirfd, const char *path, mode_t mode) {
	mlibc::infoLogger() << "\e[31mmlibc: mkdirat() ignores its mode\e[39m" << frg::endlog;
	if(!mlibc::sys_mkdirat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
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
	if (!mlibc::sys_mkfifoat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

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
	if (!mlibc::sys_mknodat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	if (int e = mlibc::sys_mknodat(dirfd, path, mode, dev); e) {
		errno = e;
		return -1;
	}

	return 0;
}

mode_t umask(mode_t) {
	mlibc::infoLogger() << "\e[31mmlibc: umask() is a no-op and always returns 0\e[39m"
			<< frg::endlog;
	return 0;
}

int utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	if(pathname == nullptr) {
		errno = EINVAL;
		return -1;
	}
	if (!mlibc::sys_utimensat) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}

	if (int e = mlibc::sys_utimensat(dirfd, pathname, times, flags); e) {
		errno = e;
		return -1;
	}

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

