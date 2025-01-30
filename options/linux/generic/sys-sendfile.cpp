
#include <sys/sendfile.h>
#include <bits/ensure.h>
#include <errno.h>

ssize_t sendfile(int, int, off_t *, size_t) {
	errno = ENOSYS;
	return -1;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

