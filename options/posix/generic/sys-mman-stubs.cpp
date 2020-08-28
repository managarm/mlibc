
#include <errno.h>
#include <sys/mman.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int mprotect(void *pointer, size_t size, int prot) {
	if(!mlibc::sys_vm_protect) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_vm_protect(pointer, size, prot); e) {
		errno = e;
		return -1;
	}
	return 0;
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
	mlibc::infoLogger() << "\e[31m" "mlibc: posix_madvise() fails unconditionally" "\e[39m"
			<< frg::endlog;
	return ENOSYS;
}

int msync(void *, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *mremap(void *pointer, size_t size, size_t new_size, int flags, ...) {
	__ensure(flags == MREMAP_MAYMOVE);

	void *window;
	if(!mlibc::sys_vm_remap) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return (void *)-1;
	}
	if(int e = mlibc::sys_vm_remap(pointer, size, new_size, &window); e) {
		errno = e;
		return (void *)-1;
	}
	return window;
}

int remap_file_pages(void *, size_t, int, size_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *mmap(void *hint, size_t size, int prot, int flags, int fd, off_t offset) {
	void *window;
	if(int e = mlibc::sys_vm_map(hint, size, prot, flags, fd, offset, &window); e) {
		errno = e;
		return (void *)-1;
	}
	return window;
}

int munmap(void *pointer, size_t size) {
	if(int e = mlibc::sys_vm_unmap(pointer, size); e) {
		errno = e;
		return -1;
	}
	return 0;
}


