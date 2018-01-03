
#include <sys/sendfile.h>
#include <mlibc/ensure.h>

ssize_t sendfile(int, int, off_t *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

