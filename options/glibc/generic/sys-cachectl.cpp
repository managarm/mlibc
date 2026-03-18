#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/cachectl.h>

#ifdef __riscv
int __riscv_flush_icache(void *start, void *end, unsigned long flags) {
	if(int e = mlibc::sysdep_or_enosys<RiscvFlushIcache>(start, end, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}
#endif
