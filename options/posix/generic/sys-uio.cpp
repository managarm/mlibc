
#include <sys/uio.h>
#include <unistd.h>

#include <bits/ensure.h>

ssize_t readv(int, const struct iovec *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

ssize_t writev(int fd, const struct iovec *iovs, int iovc) {
	__ensure(iovc);

	ssize_t written = 0;
	for(int i = 0; i < iovc; i++) {
		__ensure(iovs[i].iov_len);
		auto chunk = write(fd, iovs[i].iov_base, iovs[i].iov_len);
		// TODO: writev() should be atomic.
		// This emulation here is not correct if there is an error!
		__ensure(chunk > 0);
		written += chunk;
	}
	return written;
}

