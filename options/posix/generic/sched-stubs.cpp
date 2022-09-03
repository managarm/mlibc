
#include <bits/ensure.h>
#include <errno.h>
#include <sched.h>

#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

int sched_yield(void) {
	if(mlibc::sys_yield) {
		mlibc::sys_yield();
	}else{
		// Missing sched_yield() is not an error.
		MLIBC_MISSING_SYSDEP();
	}
	return 0;
}

int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getaffinity, -1);
	if(int e = mlibc::sys_getaffinity(pid, cpusetsize, mask); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sched_get_priority_max(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sched_get_priority_min(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int __mlibc_cpu_isset(int, cpu_set_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int __mlibc_cpu_count(cpu_set_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int unshare(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int clone(int (*)(void *), void *, int, void *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
