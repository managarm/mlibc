#include <errno.h>
#include <sys/mount.h>
#include <bits/ensure.h>
#include <sys/syscall.h>
#include <string.h>

int mount(const char *source, const char *target, int type, int flags) {
	int ret;
	size_t source_len = strlen(source);
	size_t target_len = strlen(target);
	SYSCALL6(SYSCALL_MOUNT, source, source_len, target, target_len, type, flags);
	return ret;
}

int umount(const char *target, int flags) {
	int ret;
	size_t target_len = strlen(target);
	SYSCALL3(SYSCALL_UMOUNT, target, target_len, flags);
	return ret;
}
