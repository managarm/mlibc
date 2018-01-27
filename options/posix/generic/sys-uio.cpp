
#include <sys/uio.h>

#include <bits/ensure.h>

ssize_t readv(int, const struct iovec *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
ssize_t writev(int, const struct iovec *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

