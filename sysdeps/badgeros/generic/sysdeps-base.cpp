
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>
#include <string.h>
#include <sys/syscall.h>

namespace mlibc {

void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("!!! mlibc panic !!!");
	sysdep<Exit>(-1);
	__builtin_trap();
}

void Sysdeps<LibcLog>::operator()(const char *msg) {
	__syscall_sys_log(msg, strlen(msg));
	__syscall_sys_log("\n", 1);
}

int Sysdeps<Stat>::operator()(
    fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf
) {
	if (fsfdt == fsfd_target::path) {
		fd = AT_FDCWD;
	} else if (fsfdt == fsfd_target::fd) {
		path = nullptr;
	} else {
		__ensure(fsfdt == fsfd_target::fd_path);
	}
	// TODO: This syscall subject to change.
	return -__syscall_fs_stat(fd, path, (flags & AT_SYMLINK_NOFOLLOW) == 0, statbuf);
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile("mv tp, %0" ::"r"(thread_data));
	return 0;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	auto res = __syscall_mem_map(
	    nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0
	);
	*pointer = (void *)res;
	ssize_t errno = reinterpret_cast<intptr_t>(res);
	return errno < 0 ? -errno : 0;
}

int Sysdeps<AnonFree>::operator()(void *pointer, unsigned long size) {
	__syscall_mem_unmap(pointer, size);
	return 0;
}

int Sysdeps<VmMap>::operator()(
    void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **pointer
) {
	auto res = __syscall_mem_map(hint, size, prot, flags, fd, offset);
	*pointer = (void *)res;
	ssize_t errno = reinterpret_cast<intptr_t>(res);
	return errno < 0 ? -errno : 0;
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
	return -__syscall_mem_protect(pointer, size, prot);
}

int Sysdeps<VmUnmap>::operator()(void *address, size_t size) {
	__syscall_mem_unmap(address, size);
	return 0;
}

} // namespace mlibc
