
#include <bits/ensure.h>
#include <sched.h>

#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

int sched_yield(void) {
	mlibc::sys_yield();
	return 0;
}

int sched_getaffinity(pid_t, size_t, cpu_set_t *) {
	mlibc::infoLogger() << "\e[31mmlibc: sched_getaffinity() always fails\e[39m" << frg::endlog;
	return -1;
}

int __mlibc_cpu_isset(int cpu, cpu_set_t *set) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int __mlibc_cpu_count(cpu_set_t *set) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int unshare(int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

