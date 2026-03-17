
#include <errno.h>
#include <sys/inotify.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int inotify_init(void) {
	int fd;
	if(int e = mlibc::sysdep_or_enosys<InotifyCreate>(0, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int inotify_init1(int flags) {
	int fd;
	if(int e = mlibc::sysdep_or_enosys<InotifyCreate>(flags, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int inotify_add_watch(int ifd, const char *path, uint32_t mask) {
	int wd;
	if(int e = mlibc::sysdep_or_enosys<InotifyAddWatch>(ifd, path, mask, &wd); e) {
		errno = e;
		return -1;
	}
	return wd;
}

int inotify_rm_watch(int ifd, int wd) {
	if(int e = mlibc::sysdep_or_enosys<InotifyRmWatch>(ifd, wd); e) {
		errno = e;
		return -1;
	}
	return 0;
}

