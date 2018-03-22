
#include <bits/ensure.h>
#include <sched.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

int sched_yield(void) {
	mlibc::sys_yield();
	return 0;
}

int sched_getaffinity(pid_t, size_t, cpu_set_t *) {
	frigg::infoLogger() << "\e[31mmlibc: sched_getaffinity() always fails\e[39m" << frigg::endLog;
	return -1;
}

int CPU_COUNT(cpu_set_t *set) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int unshare(int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

