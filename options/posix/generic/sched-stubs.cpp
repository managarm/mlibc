
#include <sched.h>

#include <bits/ensure.h>

#include <mlibc/sysdeps.hpp>

int sched_yield(void) {
	mlibc::sys_yield();
	return 0;
}

int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int CPU_COUNT(cpu_set_t *set) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int unshare(int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

