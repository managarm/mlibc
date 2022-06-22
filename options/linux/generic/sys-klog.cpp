#include <errno.h>
#include <sys/klog.h>

#include <bits/ensure.h>

#include <mlibc/linux-sysdeps.hpp>

int klogctl(int type, char *bufp, int len) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_klogctl, -1);
	int out;
	if (int e = mlibc::sys_klogctl(type, bufp, len, &out); e) {
		errno = e;
		return -1;
	}
	return out;
}
