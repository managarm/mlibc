#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <sys/hwprobe.h>

int __riscv_hwprobe(struct riscv_hwprobe *pairs, size_t pair_count, size_t cpusetsize, cpu_set_t *cpus, unsigned int flags) {
	int ret = mlibc::sysdep_or_enosys<RiscvHwprobe>(pairs, pair_count, cpusetsize, cpus, flags);
	if (ret)
		return -ret;

	return 0;
}
