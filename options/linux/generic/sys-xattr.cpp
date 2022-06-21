#include <errno.h>
#include <sys/xattr.h>

#include <mlibc/linux-sysdeps.hpp>
#include <bits/ensure.h>

int setxattr(const char *path, const char *name, const void *val, size_t size,
		int flags) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setxattr, -1);

	if (int e = sysdep(path, name, val, size, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int lsetxattr(const char *path, const char *name, const void *val, size_t size,
		int flags) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_lsetxattr, -1);

	if (int e = sysdep(path, name, val, size, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fsetxattr(int fd, const char *name, const void *val, size_t size,
		int flags) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fsetxattr, -1);

	if (int e = sysdep(fd, name, val, size, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t getxattr(const char *path, const char *name, void *val, size_t size) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getxattr, -1);

	ssize_t nread;
	if (int e = sysdep(path, name, val, size, &nread); e) {
		errno = e;
		return -1;
	}

	return nread;
}

ssize_t lgetxattr(const char *path, const char *name, void *val, size_t size) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_lgetxattr, -1);

	ssize_t nread;
	if (int e = sysdep(path, name, val, size, &nread); e) {
		errno = e;
		return -1;
	}

	return nread;
}

ssize_t fgetxattr(int fd, const char *name, void *val, size_t size) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fgetxattr, -1);

	ssize_t nread;
	if (int e = sysdep(fd, name, val, size, &nread); e) {
		errno = e;
		return -1;
	}

	return nread;
}

int removexattr(const char *path, const char *name) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_removexattr, -1);
	return sysdep(path, name);
}

int lremovexattr(const char *path, const char *name) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_lremovexattr, -1);
	return sysdep(path, name);
}

int fremovexattr(int fd, const char *name) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fremovexattr, -1);
	return sysdep(fd, name);
}

ssize_t listxattr(const char *path, char *list, size_t size) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_listxattr, -1);

	ssize_t nread;
	if (int e = sysdep(path, list, size, &nread); e) {
		errno = e;
		return -1;
	}
	return nread;
}

ssize_t llistxattr(const char *path, char *list, size_t size) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_llistxattr, -1);

	ssize_t nread;
	if (int e = sysdep(path, list, size, &nread); e) {
		errno = e;
		return -1;
	}
	return nread;
}

ssize_t flistxattr(int fd, char *list, size_t size) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_flistxattr, -1);

	ssize_t nread;
	if (int e = sysdep(fd, list, size, &nread); e) {
		errno = e;
		return -1;
	}
	return nread;
}
