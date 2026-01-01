
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

int sched_get_priority_max(int policy) {
	int res = 0;

	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_get_max_priority, -1);
	if(int e = sysdep(policy, &res); e) {
		errno = e;
		return -1;
	}
	return res;
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

int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setscheduler, -1);
	if(int e = sysdep(pid, policy, param); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sched_getscheduler(pid_t pid) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getscheduler, -1);
	int policy;
	if(int e = mlibc::sys_getscheduler(pid, &policy); e) {
		errno = e;
		return -1;
	}
	return policy;
}

int sched_getparam(pid_t pid, struct sched_param *param) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getparam, -1);
	if(int e = sysdep(pid, param); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sched_setparam(pid_t pid, const struct sched_param *param) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setparam, -1);
	if(int e = sysdep(pid, param); e) {
		errno = e;
		return -1;
	}
	return 0;
}
