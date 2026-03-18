#include <bits/ensure.h>
#include <devctl.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>

int posix_devctl(
    int fd, int dcmd, void *__restrict dev_data_ptr, size_t nbyte, int *__restrict dev_info_ptr
) {
	if constexpr (mlibc::IsImplemented<PosixDevctl>)
		return mlibc::sysdep_or_enosys<PosixDevctl>(fd, dcmd, dev_data_ptr, nbyte, dev_info_ptr);

	// If there is no direct implementation, we can reasonably fall back to ioctl.
	if (int e = mlibc::sysdep_or_enosys<Ioctl>(fd, dcmd, dev_data_ptr, dev_info_ptr); e)
		return e;

	return 0;
}
