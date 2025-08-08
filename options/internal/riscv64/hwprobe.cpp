#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <mlibc/internal-sysdeps.hpp>
#include <sys/hwprobe.h>

int __riscv_hwprobe(struct riscv_hwprobe *pairs, size_t pair_count, size_t cpusetsize, cpu_set_t *cpus, unsigned int flags) {
	if (!mlibc::sys_riscv_hwprobe) {
		MLIBC_MISSING_SYSDEP();
		return -ENOSYS;
	}

	int ret = mlibc::sys_riscv_hwprobe(pairs, pair_count, cpusetsize, cpus, flags);

	if (ret)
		return -ret;

	return 0;
}
