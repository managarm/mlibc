
#include <errno.h>
#include <sys/statfs.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

int statfs(const char *path, struct statfs *buf) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_statfs, -1);
	if(int e = mlibc::sys_statfs(path, buf); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fstatfs(int fd, struct statfs *buf) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fstatfs, -1);
	if (int e = mlibc::sys_fstatfs(fd, buf); e) {
		errno = e;
		return -1;
	}
	return 0;
}

[[gnu::alias("fstatfs")]] int fstatfs64(int, struct statfs64 *);

