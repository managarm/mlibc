#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/ipc.h>

key_t ftok(const char *path, int id) {
	auto sysdep_stat = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_stat, ENOSYS);

	struct stat info;
	if (int e = sysdep_stat(mlibc::fsfd_target::path, -1, path, 0, &info); e) {
		errno = e;
		return -1;
	}

	return ((info.st_ino & 0xFFFF) | ((info.st_dev & 0xFF) << 16) | ((id & 0xFF) << 24));
}
