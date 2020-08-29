
#include <sys/file.h>
#include <mlibc/posix-sysdeps.hpp>
#include <errno.h>

#include <bits/ensure.h>

int flock(int fd, int opt) {
	if(!mlibc::sys_flock) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_flock(fd, opt); e) {
		errno = e;
		return -1;
	}
	return 0;
}

