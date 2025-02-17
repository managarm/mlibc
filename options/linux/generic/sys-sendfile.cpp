#include <errno.h>
#include <sys/sendfile.h>
#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>

ssize_t sendfile(int outfd, int infd, off_t *offset, size_t size) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sendfile, -1);
	ssize_t out;
	if(int e = mlibc::sys_sendfile(outfd, infd, offset, size, &out); e) {
		errno = e;
		return -1;
	}
	return out;
}

