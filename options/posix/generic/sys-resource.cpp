
#include <errno.h>
#include <sys/resource.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int getpriority(int which, id_t who) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getpriority, -1);
	int value = 0;
	if(int e = mlibc::sys_getpriority(which, who, &value); e) {
		errno = e;
	}
	return value;
}

int setpriority(int which, id_t who, int prio) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setpriority, -1);
	if(int e = mlibc::sys_setpriority(which, who, prio); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getrusage(int scope, struct rusage *usage) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getrusage, -1);
	if(int e = mlibc::sys_getrusage(scope, usage); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getrlimit(int resource, struct rlimit *limit) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getrlimit, -1);
	if(int e = mlibc::sys_getrlimit(resource, limit); e) {
		errno = e;
		return -1;
	}
	return 0;
}

[[gnu::alias("getrlimit")]] int getrlimit64(int resource, struct rlimit *limit);

int setrlimit(int resource, const struct rlimit *limit) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setrlimit, -1);
	if(int e = mlibc::sys_setrlimit(resource, limit); e) {
		errno = e;
		return -1;
	}
	return 0;
}

[[gnu::alias("setrlimit")]] int setrlimit64(int resource, const struct rlimit *limit);

int prlimit(pid_t, int, const struct rlimit *, struct rlimit *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
