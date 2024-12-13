
#include <errno.h>
#include <bits/ensure.h>
#include <fcntl.h>
#include <stdarg.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int creat(const char *pathname, mode_t mode) {
	return open(pathname, O_CREAT|O_WRONLY|O_TRUNC, mode);
}

int fallocate(int, int, off_t, off_t) {
	mlibc::infoLogger() << "mlibc: fallocate() is a no-op" << frg::endlog;
	errno = ENOSYS;
	return -1;
}

int fcntl(int fd, int command, ...) {
	va_list args;
	va_start(args, command);
	int result;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fcntl, -1);
	if(int e = mlibc::sys_fcntl(fd, command, args, &result); e) {
		errno = e;
		return -1;
	}
	va_end(args);
	return result;
}

int openat(int dirfd, const char *pathname, int flags, ...) {
	va_list args;
	va_start(args, flags);
	mode_t mode = 0;
	int fd;

	if((flags & (O_CREAT | O_TMPFILE)))
		mode = va_arg(args, mode_t);

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_openat, -1);
	if(int e = mlibc::sys_openat(dirfd, pathname, flags, mode, &fd); e) {
		errno = e;
		return -1;
	}
	va_end(args);
	return fd;
}

int posix_fadvise(int fd, off_t offset, off_t length, int advice) {
	if(!mlibc::sys_fadvise) {
		mlibc::infoLogger() << "mlibc: fadvise() ignored due to missing sysdep" << frg::endlog;
		return 0;
	}

	// posix_fadvise() returns an error instead of setting errno.
	return mlibc::sys_fadvise(fd, offset, length, advice);
}

int posix_fallocate(int fd, off_t offset, off_t size) {
	// posix_fallocate() returns an error instead of setting errno.
	if(!mlibc::sys_fallocate) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}
	return mlibc::sys_fallocate(fd, offset, size);
}

// This is a linux extension
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

int open(const char *pathname, int flags, ...) {
	mode_t mode = 0;

	if ((flags & O_CREAT) || (flags & O_TMPFILE)) {
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, mode_t);
		va_end(args);
	}

	int fd;
	if(int e = mlibc::sys_open(pathname, flags, mode, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

[[gnu::alias("open")]] int open64(const char *pathname, int flags, ...);