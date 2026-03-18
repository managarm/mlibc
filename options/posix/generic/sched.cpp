
#include <bits/ensure.h>
#include <errno.h>
#include <limits.h>
#include <sched.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int sched_yield(void) {
	if constexpr (mlibc::IsImplemented<Yield>) {
		mlibc::sysdep_or_enosys<Yield>();
	}else{
		// Missing sched_yield() is not an error.
		MLIBC_MISSING_SYSDEP();
	}
	return 0;
}

int sched_get_priority_max(int policy) {
	int res = 0;
	if(int e = mlibc::sysdep_or_enosys<GetMaxPriority>(policy, &res); e) {
		errno = e;
		return -1;
	}
	return res;
}

int sched_get_priority_min(int policy) {
	int res = 0;
	if(int e = mlibc::sysdep_or_enosys<GetMinPriority>(policy, &res); e) {
		errno = e;
		return -1;
	}
	return res;
}

int sched_setscheduler(pid_t pid, int policy, const struct sched_param *param) {
	if(int e = mlibc::sysdep_or_enosys<SetScheduler>(pid, policy, param); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sched_getscheduler(pid_t pid) {
	int policy;
	if(int e = mlibc::sysdep_or_enosys<GetScheduler>(pid, &policy); e) {
		errno = e;
		return -1;
	}
	return policy;
}

int sched_getparam(pid_t pid, struct sched_param *param) {
	if(int e = mlibc::sysdep_or_enosys<GetParam>(pid, param); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int sched_setparam(pid_t pid, const struct sched_param *param) {
	if(int e = mlibc::sysdep_or_enosys<SetParam>(pid, param); e) {
		errno = e;
		return -1;
	}
	return 0;
}
