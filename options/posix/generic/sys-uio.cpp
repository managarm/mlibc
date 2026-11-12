
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

ssize_t readv(int fd, const struct iovec *iovs, int iovc) {
	ssize_t read_bytes = 0;
	if (int e = mlibc::sysdep_or_enosys<Readv>(fd, iovs, iovc, &read_bytes); e) {
		errno = e;
		return -1;
	}

	return read_bytes;
}

ssize_t writev(int fd, const struct iovec *iovs, int iovc) {
	ssize_t written = 0;
	if (int e = mlibc::sysdep_or_enosys<Writev>(fd, iovs, iovc, &written); e) {
		errno = e;
		return -1;
	}

	return written;
}

ssize_t preadv(int, const struct iovec *, int, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t pwritev(int, const struct iovec *, int, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
