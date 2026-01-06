#include <bits/ensure.h>
#include <devctl.h>
#include <errno.h>
#include <mlibc/posix-sysdeps.hpp>

int posix_devctl(
    int fd, int dcmd, void *__restrict dev_data_ptr, size_t nbyte, int *__restrict dev_info_ptr
) {
	if (mlibc::sys_posix_devctl)
		return mlibc::sys_posix_devctl(fd, dcmd, dev_data_ptr, nbyte, dev_info_ptr);

	// If there is no direct implementation, we can reasonably fall back to ioctl.
	if (!mlibc::sys_ioctl) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}

	if (int e = mlibc::sys_ioctl(fd, dcmd, dev_data_ptr, dev_info_ptr); e)
		return e;

	return 0;
}
