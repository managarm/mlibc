
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

