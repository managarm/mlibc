
#include <errno.h>
#include <sys/resource.h>

#include <bits/ensure.h>
#include <mlibc/posix-sysdeps.hpp>

int getpriority(int, id_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setpriority(int, id_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

int setrlimit(int, const struct rlimit *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
