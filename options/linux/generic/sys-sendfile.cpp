#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/sendfile.h>

ssize_t sendfile(int outfd, int infd, off_t *offset, size_t size) {
	ssize_t out;
	if(int e = mlibc::sysdep_or_enosys<Sendfile>(outfd, infd, offset, size, &out); e) {
		errno = e;
		return -1;
	}
	return out;
}

