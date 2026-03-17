#include <errno.h>
#include <sys/klog.h>

#include <bits/ensure.h>

#include <mlibc/all-sysdeps.hpp>

int klogctl(int type, char *bufp, int len) {
	int out;
	if (int e = mlibc::sysdep_or_enosys<Klogctl>(type, bufp, len, &out); e) {
		errno = e;
		return -1;
	}
	return out;
}
