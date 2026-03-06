#include <bits/linux/linux_unistd.h>
#include <bits/ensure.h>

#include <errno.h>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/linux-sysdeps.hpp>
#include <unistd.h>

int vhangup(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int syncfs(int fd) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_syncfs, -1);
	if(int e = mlibc::sys_syncfs(fd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

ssize_t copy_file_range(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t count, unsigned int flags) {
	ssize_t bytes_copied = 0;
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_copy_file_range, -1);
	if(int e = sysdep(fd_in, off_in, fd_out, off_out, count, flags, &bytes_copied); e < 0) {
		errno = e;
		return -1;
	}
	return bytes_copied;
}
