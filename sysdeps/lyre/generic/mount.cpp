#include <errno.h>
#include <sys/mount.h>
#include <bits/ensure.h>

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	return 0;
}

int umount(const char *target) {
	return umount2(target, 0);
}

int umount2(const char *target, int flags) {
	return 0;
}
