
#include <sys/mman.h>
#include <mlibc/ensure.h>

void *mmap(void *, size_t, int, int, int, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int mprotect(void *, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int munmap(void *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}


int mlock(const void *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int mlockall(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int munlock(const void *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int munlockall(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}


int posix_madvise(void *, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int msync(void *, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}


