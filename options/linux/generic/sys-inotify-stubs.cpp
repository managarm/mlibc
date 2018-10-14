
#include <bits/ensure.h>
#include <sys/inotify.h>

#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

int inotify_init(void) {
	int fd;
	if(mlibc::sys_inotify_create(0, &fd))
		return -1;
	return fd;
}

int inotify_init1(int flags) {
	int fd;
	if(mlibc::sys_inotify_create(0, &fd))
		return -1;
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

