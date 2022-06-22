
#include <bits/ensure.h>
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

int sched_getaffinity(pid_t, size_t, cpu_set_t *) {
	mlibc::infoLogger() << "\e[31mmlibc: sched_getaffinity() always fails\e[39m" << frg::endlog;
	return -1;
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

int __mlibc_cpu_count(size_t setsize, cpu_set_t *set) {
	size_t i, j, cnt=0;
	const unsigned char *p = reinterpret_cast<const unsigned char *>(set);
	for (i=0; i<setsize; i++) for (j=0; j<8; j++)
		if (p[i] & (1<<j)) cnt++;
	return cnt;
}

int unshare(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int clone(int (*)(void *), void *, int, void *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
