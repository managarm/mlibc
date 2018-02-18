
#include <sys/mman.h>
#include <bits/ensure.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

int mprotect(void *, size_t, int) {
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

void *mremap(void *, size_t, size_t, int, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int remap_file_pages(void *, size_t, int, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *mmap(void *hint, size_t size, int prot, int flags, int fd, off_t offset) {
	void *window;
	if(mlibc::sys_vm_map(hint, size, prot, flags, fd, offset, &window))
		return (void *) -1;
	return window;
}

int munmap(void *pointer, size_t size) {
	if(mlibc::sys_vm_unmap(pointer, size))
		return -1;
	return 0;
}


