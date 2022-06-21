#include <errno.h>
#include <sys/reboot.h>
#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>

int reboot(int what) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_reboot, -1);
	if (int e = mlibc::sys_reboot(what); e) {
		errno = e;
		return -1;
	}
	return 0;
}
