#include <menix/syscall.hpp>
#include <sys/mount.h>

namespace mlibc {

int mount(const char *type, const char *dir, int flags, void *data) {
	return menix_syscall(SYSCALL_MOUNT, (size_t)type, (size_t)dir, flags, (size_t)data).error;
}

int unmount(const char *dir, int flags) {
	return menix_syscall(SYSCALL_UMOUNT, (size_t)dir, flags).error;
}

} // namespace mlibc
