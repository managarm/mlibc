
#include <errno.h>
#include <sys/mount.h>

#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	if(!mlibc::sys_mount) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_mount(source, target, fstype, flags, data); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int umount(const char *target) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int umount2(const char *target, int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
