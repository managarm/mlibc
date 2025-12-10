#include <abi-bits/fcntl.h>
#include <bits/posix/posix_time.h>
#include <bits/ensure.h>
#include <mlibc/posix-sysdeps.hpp>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

int timer_getoverrun(timer_t t) {
	int out = 0;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_timer_getoverrun, -1);
	if(int e = mlibc::sys_timer_getoverrun(t, &out); e) {
		errno = e;
		return -1;
	}
	return out;
}

int utimes(const char *filename, const struct timeval times[2]) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_utimensat, -1);
	struct timespec time[2];
	if(times == nullptr) {
		time[0].tv_sec = UTIME_NOW;
		time[0].tv_nsec = UTIME_NOW;
		time[1].tv_sec = UTIME_NOW;
		time[1].tv_nsec = UTIME_NOW;
	} else {
		time[0].tv_sec = times[0].tv_sec;
		time[0].tv_nsec = times[0].tv_usec * 1000;
		time[1].tv_sec = times[1].tv_sec;
		time[1].tv_nsec = times[1].tv_usec * 1000;
	}

	if (int e = mlibc::sys_utimensat(AT_FDCWD, filename, time, 0); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int futimes(int fd, const struct timeval times[2]) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_utimensat, -1);
	struct timespec time[2];
	if(times == nullptr) {
		time[0].tv_sec = UTIME_NOW;
		time[0].tv_nsec = UTIME_NOW;
		time[1].tv_sec = UTIME_NOW;
		time[1].tv_nsec = UTIME_NOW;
	} else {
		time[0].tv_sec = times[0].tv_sec;
		time[0].tv_nsec = times[0].tv_usec * 1000;
		time[1].tv_sec = times[1].tv_sec;
		time[1].tv_nsec = times[1].tv_usec * 1000;
	}

	if (int e = mlibc::sys_utimensat(fd, "", time, AT_EMPTY_PATH); e) {
		errno = e;
		return -1;
	}

	return 0;
}

int lutimes(const char *filename, const struct timeval tv[2]) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_utimensat, -1);
	struct timespec time[2];
	if(tv == nullptr) {
		time[0].tv_sec = UTIME_NOW;
		time[0].tv_nsec = UTIME_NOW;
		time[1].tv_sec = UTIME_NOW;
		time[1].tv_nsec = UTIME_NOW;
	} else {
		time[0].tv_sec = tv[0].tv_sec;
		time[0].tv_nsec = tv[0].tv_usec * 1000;
		time[1].tv_sec = tv[1].tv_sec;
		time[1].tv_nsec = tv[1].tv_usec * 1000;
	}

	if (int e = mlibc::sys_utimensat(AT_FDCWD, filename, time, AT_SYMLINK_NOFOLLOW); e) {
		errno = e;
		return -1;
	}

	return 0;
}
