
#include <errno.h>
#include <sys/resource.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int getpriority(int which, id_t who) {
	if(!mlibc::sys_getpriority) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	int value = 0;
	if(int e = mlibc::sys_getpriority(which, who, &value); e) {
		errno = e;
	}
	return value;
}

int setpriority(int which, id_t who, int prio) {
	if(!mlibc::sys_setpriority) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_setpriority(which, who, prio); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getrusage(int scope, struct rusage *usage) {
	if(!mlibc::sys_getrusage) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_getrusage(scope, usage); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getrlimit(int resource, struct rlimit *limit) {
	if(!mlibc::sys_getrlimit) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_getrlimit(resource, limit); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setrlimit(int resource, const struct rlimit *limit) {
	if(!mlibc::sys_setrlimit) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_setrlimit(resource, limit); e) {
		errno = e;
		return -1;
	}
	return 0;
}
