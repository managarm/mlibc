
#include <fcntl.h>

#include <mlibc/ensure.h>

int creat(const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fcntl(int, int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int open(const char *, int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int openat(int, const char *, int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int posix_fadvise(int, off_t, off_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int posix_fallocate(int, off_t, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

