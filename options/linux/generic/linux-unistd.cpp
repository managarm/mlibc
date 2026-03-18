#include <bits/linux/linux_unistd.h>
#include <bits/ensure.h>

#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <unistd.h>

int vhangup(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int syncfs(int fd) {
	if(int e = mlibc::sysdep_or_enosys<Syncfs>(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t copy_file_range(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t count, unsigned int flags) {
	ssize_t bytes_copied = 0;
	if(int e = mlibc::sysdep_or_enosys<CopyFileRange>(fd_in, off_in, fd_out, off_out, count, flags, &bytes_copied); e < 0) {
		errno = e;
		return -1;
	}
	return bytes_copied;
}
