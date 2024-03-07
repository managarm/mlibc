#include <errno.h>
#include <sys/stat.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

int statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_statx, -1);
	if(int e = mlibc::sys_statx(dirfd, pathname, flags, mask, statxbuf); e) {
		errno = e;
		return -1;
	}
	return 0;
}

