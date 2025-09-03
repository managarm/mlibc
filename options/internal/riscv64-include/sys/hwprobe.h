#ifndef _MLIBC_SYS_HWPROBE_H
#define _MLIBC_SYS_HWPROBE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/riscv-hwprobe.h>
#include <bits/cpu_set.h>
#include <bits/size_t.h>

int __riscv_hwprobe(struct riscv_hwprobe *pairs, size_t pair_count, size_t cpusetsize, cpu_set_t *cpus,
	unsigned int flags) __attribute__((access(read_write, 1, 2)));

typedef int (*__riscv_hwprobe_t)(struct riscv_hwprobe *pairs, size_t pair_count,
	size_t cpusetsize, cpu_set_t *cpus, unsigned int flags) __attribute__((access(read_write, 1, 2)));

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_SYS_HWPROBE_H */
