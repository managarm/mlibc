#include <errno.h>
#include <sys/swap.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

int swapon(const char *path, int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_swapon, -1);
	if(int e = mlibc::sys_swapon(path, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int swapoff(const char *path) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_swapoff, -1);
	if(int e = mlibc::sys_swapoff(path); e) {
		errno = e;
		return -1;
	}
	return 0;
}
