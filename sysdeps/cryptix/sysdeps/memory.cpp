#include <cryptix/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/mman.h>

namespace mlibc {
int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	return sysdep<VmMap>(
	    nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, pointer
	);
}
int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
	return sysdep<VmUnmap>(pointer, size);
}

int Sysdeps<VmMap>::operator()(
    void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window
) {
	auto ret = Syscall(SYS_MMAP, hint, size, prot, flags, fd, offset);
	if (auto e = syscall_error(ret); e)
		return e;

	*window = reinterpret_cast<void *>(ret);
	return 0;
}
int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	auto ret = Syscall(SYS_MUNMAP, pointer, size);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
	auto ret = Syscall(SYS_MPROTECT, pointer, size, prot);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
} // namespace mlibc
