
#include <utime.h>
#include <fcntl.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

int utime(const char *filename, const struct utimbuf *times) {
	struct timespec time[2];
	if(times) {
		time[0].tv_sec = times->actime;
		time[0].tv_nsec = 0;
		time[1].tv_sec = times->modtime;
		time[1].tv_nsec = 0;
	} else {
		time[0].tv_sec = UTIME_NOW;
		time[0].tv_nsec = UTIME_NOW;
		time[1].tv_sec = UTIME_NOW;
		time[1].tv_nsec = UTIME_NOW;
	}

	if (int e = mlibc::sysdep_or_enosys<Utimensat>(AT_FDCWD, filename, time, 0); e) {
		errno = e;
		return -1;
	}

	return 0;
}

