#include <errno.h>
#include <sys/mount.h>
#include <bits/ensure.h>
#include <sys/syscall.h>
#include <string.h>

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	int ret, errno;
	size_t source_len = strlen(source);
	size_t target_len = strlen(target);
	int val;
	if (!strcmp(fstype, "ext")) {
		val = 1;
	} else if (!strcmp(fstype, "fat32")) {
		val = 2;
	} else {
		return EINVAL;
	}

	SYSCALL6(SYSCALL_MOUNT, source, source_len, target, target_len, val, flags);
	return errno;
}

int umount(const char *target) {
	return umount2(target, 0);
}

int umount2(const char *target, int flags) {
	int ret, errno;
	size_t target_len = strlen(target);
	SYSCALL3(SYSCALL_UMOUNT, target, target_len, flags);
	return errno;
}
