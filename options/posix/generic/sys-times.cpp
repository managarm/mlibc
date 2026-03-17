
#include <errno.h>
#include <sys/times.h>

#include <bits/ensure.h>
#include <internal-config.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

clock_t times(struct tms *tms) {
	clock_t ret;
	if(int e = mlibc::sysdep_or_enosys<Times>(tms, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}

