
#include <errno.h>
#include <sys/times.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <internal-config.h>
#include <mlibc/posix-sysdeps.hpp>

clock_t times(struct tms *tms) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_times, -1);
	clock_t ret;
	if(int e = mlibc::sys_times(tms, &ret); e) {
		errno = e;
		return -1;
	}
	return ret;
}

