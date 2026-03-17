
#include <errno.h>
#include <bits/ensure.h>

#include <stdlib.h>

#include <mlibc/all-sysdeps.hpp>

int getloadavg(double *samples, int nsample) {
	if (nsample < 0) {
		errno = EINVAL;
		return -1;
	}
	if (nsample > 3) {
		nsample = 3;
	}
	double s[3];
	if (int e = mlibc::sysdep_or_enosys<GetLoadavg>(s); e) {
		errno = e;
		return -1;
	}
	for (int i = 0; i < nsample; i++) {
		samples[i] = s[i];
	}
	return nsample;
}
