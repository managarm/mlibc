#include <bits/ensure.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/syscall.h>

int mount(
    const char *source,
    const char *target,
    const char *fstype,
    unsigned long flags,
    const void *data
) {
	int e = SYSCALL5(SYS_MOUNT, source, target, fstype, flags, data);

	if (e < 0) {
		errno = -e;
		return -1;
	}

	return 0;
}

int umount(const char *target) { return umount2(target, 0); }

int umount2(const char *target, int flags) {
	errno = ENOSYS;
	return -1;
}
