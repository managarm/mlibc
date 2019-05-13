
#include <errno.h>
#include <sys/inotify.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

int inotify_init(void) {
	int fd;
	if(!mlibc::sys_inotify_create) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_inotify_create(0, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int inotify_init1(int flags) {
	int fd;
	if(!mlibc::sys_inotify_create) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_inotify_create(0, &fd); e) {
		errno = e;
		return -1;
	}
	return fd;
}

int inotify_add_watch(int, const char *, unsigned int) {
	mlibc::infoLogger() << "\e[31mmlibc: inotify_add_watch() is broken\e[39m" << frg::endlog;
	return 0; // TODO: Return a non-negative watch descriptor.
}

int inotify_rm_watch(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

