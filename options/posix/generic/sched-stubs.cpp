
#include <sched.h>

#include <bits/ensure.h>

#include <mlibc/sysdeps.hpp>

int sched_yield(void) {
	mlibc::sys_yield();
	return 0;
}

