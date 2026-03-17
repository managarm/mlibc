
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>

ssize_t readv(int fd, const struct iovec *iovs, int iovc) {
	ssize_t read_bytes = 0;
	if (int e = mlibc::sysdep_or_enosys<Readv>(fd, iovs, iovc, &read_bytes); e) {
		errno = e;
		return -1;
	}

	return read_bytes;
}

ssize_t writev(int fd, const struct iovec *iovs, int iovc) {
	__ensure(iovc);

	ssize_t written = 0;

	if constexpr (mlibc::IsImplemented<Writev>) {
		int e = mlibc::sysdep_or_panic<Writev>(fd, iovs, iovc, &written);
		if(e) {
			errno = e;
			return -1;
		}
		return written;
	}

	// TODO: this implementation is not safe to use in signal contexts
	mlibc::infoLogger() << "mlibc: falling back to signal-unsafe writev implementation!" << frg::endlog;
	size_t bytes = 0;
	for(int i = 0; i < iovc; i++) {
		if(SSIZE_MAX - bytes < iovs[i].iov_len) {
			errno = EINVAL;
			return -1;
		}
		bytes += iovs[i].iov_len;
	}
	frg::vector<char, MemoryAllocator> buffer{getAllocator()};
	buffer.resize(bytes);

	size_t to_copy = bytes;
	char *bp = buffer.data();
	for(int i = 0; i < iovc; i++) {
		size_t copy = frg::min(iovs[i].iov_len, to_copy);

		bp = (char *)memcpy((void *)bp, (void *)iovs[i].iov_base, copy) + copy;

		to_copy -= copy;
		if(to_copy == 0)
			break;
	}

	written = write(fd, buffer.data(), bytes);
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
