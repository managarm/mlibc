#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <errno.h>
#include <bits/ensure.h>
#include <fcntl.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int fallocate(int, int, off_t, off_t) {
	mlibc::infoLogger() << "mlibc: fallocate() is a no-op" << frg::endlog;
	errno = ENOSYS;
	return -1;
}

int name_to_handle_at(int dirfd, const char *pathname, struct file_handle *handle, int *mount_id, int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_name_to_handle_at, -1);
	if(int e = mlibc::sys_name_to_handle_at(dirfd, pathname, handle, mount_id, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int open_by_handle_at(int, struct file_handle *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t splice(int in_fd, off_t *in_off, int out_fd, off_t *out_off, size_t size, unsigned int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_splice, -1);
	ssize_t ret;
	if(int e = mlibc::sys_splice(in_fd, in_off, out_fd, out_off, size, flags, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}

ssize_t vmsplice(int, const struct iovec *, size_t, unsigned int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
