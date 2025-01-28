#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/glibc-sysdeps.hpp>
#include <sys/cachectl.h>

#ifdef __riscv
int __riscv_flush_icache(void *start, void *end, unsigned long flags) {
	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_riscv_flush_icache, -1);

	if(int e = sysdep(start, end, flags); e) {
		errno = e;
		return -1;
	}
	return 0;
}
#endif
