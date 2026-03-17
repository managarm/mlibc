
#include <errno.h>
#include <bits/ensure.h>
#include <sys/stat.h>

#include <mlibc-config.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int chmod(const char *pathname, mode_t mode) {
	if(int e = mlibc::sysdep_or_enosys<Chmod>(pathname, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchmod(int fd, mode_t mode) {
	if(int e = mlibc::sysdep_or_enosys<Fchmod>(fd, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Fchmodat>(dirfd, pathname, mode, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fstatat(int dirfd, const char *path, struct stat *result, int flags) {
	if(int e = mlibc::sysdep_or_enosys<Stat>(mlibc::fsfd_target::fd_path, dirfd, path, flags, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int futimens(int fd, const struct timespec times[2]) {

	if (int e = mlibc::sysdep_or_enosys<Utimensat>(fd, nullptr, times, 0); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int mkdir(const char *path, mode_t mode) {
	if(int e = mlibc::sysdep_or_enosys<Mkdir>(path, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mkdirat(int dirfd, const char *path, mode_t mode) {
	if(int e = mlibc::sysdep_or_enosys<Mkdirat>(dirfd, path, mode); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int mkfifo(const char *path, mode_t mode) {
	return mkfifoat(AT_FDCWD, path, mode);
}

int mkfifoat(int dirfd, const char *path, mode_t mode) {
	if (int e = mlibc::sysdep_or_enosys<Mkfifoat>(dirfd, path, mode); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int mknod(const char *path, mode_t mode, dev_t dev) {
	return mknodat(AT_FDCWD, path, mode, dev);
}

int mknodat(int dirfd, const char *path, mode_t mode, dev_t dev) {
	if (int e = mlibc::sysdep_or_enosys<Mknodat>(dirfd, path, mode, dev); e) {
		errno = e;
		return -1;
	}

	return 0;
}

mode_t umask(mode_t mode) {
	mode_t old;
	if (int e = mlibc::sysdep_or_enosys<Umask>(mode, &old); e) {
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

	if (int e = mlibc::sysdep_or_enosys<Utimensat>(dirfd, pathname, times, flags); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int stat(const char *path, struct stat *result) {
	if(int e = mlibc::sysdep_or_enosys<Stat>(mlibc::fsfd_target::path, -1, path, 0, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int lstat(const char *path, struct stat *result) {
	if(int e = mlibc::sysdep_or_enosys<Stat>(mlibc::fsfd_target::path,
			-1, path, AT_SYMLINK_NOFOLLOW, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("lstat")]] int lstat64(const char *path, struct stat64 *result);
#endif /* !__MLIBC_LINUX_OPTION */

int fstat(int fd, struct stat *result) {
	if(int e = mlibc::sysdep_or_enosys<Stat>(mlibc::fsfd_target::fd, fd, "", 0, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("fstat")]] int fstat64(int fd, struct stat64 *result);
#endif /* !__MLIBC_LINUX_OPTION */
