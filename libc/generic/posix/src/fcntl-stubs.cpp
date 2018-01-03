
#include <fcntl.h>

#include <mlibc/ensure.h>

int creat(const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// fcntl() is provided by the platform
// open() is provided by the platform
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

// This is a linux extension
int name_to_handle_at(int, const char *, struct file_handle *, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int open_by_handle_at(int, struct file_handle *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

