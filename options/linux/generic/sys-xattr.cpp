#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/xattr.h>

int setxattr(const char *path, const char *name, const void *val, size_t size,
		int flags) {
	if (int e = mlibc::sysdep_or_enosys<Setxattr>(path, name, val, size, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int lsetxattr(const char *path, const char *name, const void *val, size_t size,
		int flags) {
	if (int e = mlibc::sysdep_or_enosys<Lsetxattr>(path, name, val, size, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fsetxattr(int fd, const char *name, const void *val, size_t size,
		int flags) {
	if (int e = mlibc::sysdep_or_enosys<Fsetxattr>(fd, name, val, size, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t getxattr(const char *path, const char *name, void *val, size_t size) {
	ssize_t nread;
	if (int e = mlibc::sysdep_or_enosys<Getxattr>(path, name, val, size, &nread); e) {
		errno = e;
		return -1;
	}

	return nread;
}

ssize_t lgetxattr(const char *path, const char *name, void *val, size_t size) {
	ssize_t nread;
	if (int e = mlibc::sysdep_or_enosys<Lgetxattr>(path, name, val, size, &nread); e) {
		errno = e;
		return -1;
	}

	return nread;
}

ssize_t fgetxattr(int fd, const char *name, void *val, size_t size) {
	ssize_t nread;
	if (int e = mlibc::sysdep_or_enosys<Fgetxattr>(fd, name, val, size, &nread); e) {
		errno = e;
		return -1;
	}

	return nread;
}

int removexattr(const char *path, const char *name) {
	if (int e = mlibc::sysdep_or_enosys<Removexattr>(path, name); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int lremovexattr(const char *path, const char *name) {
	if (int e = mlibc::sysdep_or_enosys<Lremovexattr>(path, name); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fremovexattr(int fd, const char *name) {
	if (int e = mlibc::sysdep_or_enosys<Fremovexattr>(fd, name); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t listxattr(const char *path, char *list, size_t size) {
	ssize_t nread;
	if (int e = mlibc::sysdep_or_enosys<Listxattr>(path, list, size, &nread); e) {
		errno = e;
		return -1;
	}
	return nread;
}

ssize_t llistxattr(const char *path, char *list, size_t size) {
	ssize_t nread;
	if (int e = mlibc::sysdep_or_enosys<Llistxattr>(path, list, size, &nread); e) {
		errno = e;
		return -1;
	}
	return nread;
}

ssize_t flistxattr(int fd, char *list, size_t size) {
	ssize_t nread;
	if (int e = mlibc::sysdep_or_enosys<Flistxattr>(fd, list, size, &nread); e) {
		errno = e;
		return -1;
	}
	return nread;
}
