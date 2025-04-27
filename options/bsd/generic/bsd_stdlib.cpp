
#include <errno.h>
#include <bits/ensure.h>

#include <stdlib.h>

#include <mlibc/bsd-sysdeps.hpp>

int getloadavg(double *samples, int nsample) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getloadavg, -1);
	if (nsample < 0) {
		errno = EINVAL;
		return -1;
	}
	if (nsample > 3) {
		nsample = 3;
	}
	double s[3];
	if (int e = sysdep(s); e) {
		errno = e;
		return -1;
	}
	for (int i = 0; i < nsample; i++) {
		samples[i] = s[i];
	}
	return nsample;
}
