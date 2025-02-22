#include <bits/ensure.h>
#include <errno.h>
#include <sched.h>

#include <mlibc/linux-sysdeps.hpp>
#include <mlibc/posix-sysdeps.hpp>

int sched_getcpu(void) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getcpu, -1);
	int cpu;
	if(int e = mlibc::sys_getcpu(&cpu); e) {
		errno = e;
		return -1;
	}
	return cpu;
}

int setns(int fd, int nstype) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setns, -1);
	if(int e = mlibc::sys_setns(fd, nstype); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sched_getscheduler(pid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getaffinity, -1);
	if(int e = mlibc::sys_getaffinity(pid, cpusetsize, mask); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int unshare(int flags) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_unshare, -1);
	if(int e = mlibc::sys_unshare(flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sched_setaffinity(pid_t, size_t, const cpu_set_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int clone(int (*)(void *), void *, int, void *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
