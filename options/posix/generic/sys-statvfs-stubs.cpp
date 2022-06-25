#include <errno.h>
#include <sys/statvfs.h>

#include <bits/ensure.h>
#include <mlibc/posix-sysdeps.hpp>

int statvfs(const char *path, struct statvfs *out) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_statvfs, -1);
	if(int e = mlibc::sys_statvfs(path, out); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fstatvfs(int fd, struct statvfs *out) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fstatvfs, -1);
	if(int e = mlibc::sys_fstatvfs(fd, out); e) {
		errno = e;
		return -1;
	}
	return 0;
}

