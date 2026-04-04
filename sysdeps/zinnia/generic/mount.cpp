#include <sys/mount.h>
#include <zinnia/syscall.hpp>

int mount(const char *type, const char *dir, int flags, void *data) {
	return zinnia_syscall(SYSCALL_MOUNT, (size_t)type, (size_t)dir, flags, (size_t)data).error;
}

int unmount(const char *dir, int flags) {
	return zinnia_syscall(SYSCALL_UMOUNT, (size_t)dir, flags).error;
}
