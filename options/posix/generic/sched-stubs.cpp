
#include <bits/ensure.h>
#include <errno.h>
#include <limits.h>
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

int sched_get_priority_min(int policy) {
	int res = 0;

	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_get_min_priority, -1);
	if(int e = sysdep(policy, &res); e) {
		errno = e;
		return -1;
	}
	return res;
}

int __mlibc_cpu_isset(int, cpu_set_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int __mlibc_cpu_count(const cpu_set_t *set) {
	size_t count = 0;
	const unsigned char *ptr = reinterpret_cast<const unsigned char *>(set);

	for(size_t i = 0; i < sizeof(cpu_set_t); i++) {
		for(size_t bit = 0; bit < CHAR_BIT; bit++) {
			if((1 << bit) & ptr[i])
				count++;
		}
	}

	return count;
}

int unshare(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int clone(int (*)(void *), void *, int, void *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sched_setscheduler(pid_t, int, const struct sched_param *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sched_setaffinity(pid_t, size_t, const cpu_set_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sched_getscheduler(pid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int sched_getparam(pid_t, struct sched_param *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
