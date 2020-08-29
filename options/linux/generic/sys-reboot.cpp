#include <errno.h>
#include <sys/reboot.h>
#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>

int reboot(int what) {
	if (!mlibc::sys_reboot) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if (int e = mlibc::sys_reboot(what); e) {
		errno = e;
		return -1;
	}
	return 0;
}
