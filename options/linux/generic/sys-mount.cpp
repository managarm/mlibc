
#include <errno.h>
#include <sys/mount.h>

#include <bits/ensure.h>
#include <mlibc/sysdeps.hpp>

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	if(int e = mlibc::sys_mount(source, target, fstype, flags, data); e) {
		errno = e;
		return -1;
	}
	return 0;
}


