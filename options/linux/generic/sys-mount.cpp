
#include <sys/mount.h>
#include <bits/ensure.h>

#include <mlibc/sysdeps.hpp>

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	return mlibc::sys_mount(source, target, fstype, flags, data);
}


