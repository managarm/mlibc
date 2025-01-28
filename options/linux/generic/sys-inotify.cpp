
#include <errno.h>
#include <sys/inotify.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

int inotify_init(void) {
	int fd;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_inotify_create, -1);
	if(int e = mlibc::sys_inotify_create(0, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int inotify_init1(int flags) {
	int fd;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_inotify_create, -1);
	if(int e = mlibc::sys_inotify_create(flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int inotify_add_watch(int ifd, const char *path, uint32_t mask) {
	int wd;
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_inotify_add_watch, -1);
	if(int e = mlibc::sys_inotify_add_watch(ifd, path, mask, &wd); e) {
		errno = e;
		return -1;
	}
	return wd;
}

int inotify_rm_watch(int ifd, int wd) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_inotify_rm_watch, -1);
	if(int e = mlibc::sys_inotify_rm_watch(ifd, wd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

