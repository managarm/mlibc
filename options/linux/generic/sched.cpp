#include <bits/ensure.h>
#include <errno.h>
#include <sched.h>

#include <mlibc/linux-sysdeps.hpp>

int sched_getcpu(void) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getcpu, -1);
	int cpu;
	if(int e = mlibc::sys_getcpu(&cpu); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int setns(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
