#include <stdint.h>

static uint64_t foo_impl(void) {
    return 420;
}

#if defined(__riscv) && !defined(USE_HOST_LIBC)
#include <sys/hwprobe.h>
#elif defined(__riscv)
int __riscv_hwprobe(struct riscv_hwprobe *pairs, size_t pair_count, size_t cpusetsize, cpu_set_t *cpus,
	unsigned int flags) __attribute__((access(read_write, 1, 2)));

typedef int (*__riscv_hwprobe_t)(struct riscv_hwprobe *pairs, size_t pair_count,
	size_t cpusetsize, cpu_set_t *cpus, unsigned int flags) __attribute__((access(read_write, 1, 2)));
#endif

#if defined(__riscv)
#include <stdlib.h>

static uint64_t (*foo_resolver(long long, __riscv_hwprobe_t hwprobe, void *))(void) {
	if (!hwprobe)
		return NULL;
	struct riscv_hwprobe pairs[1] = {
		{ .key = 4, .value = 0 }
	};
	hwprobe(pairs, 1, 0, 0, 0);
	return foo_impl;
}
#else
static uint64_t (*foo_resolver(void))(void) {
    return foo_impl;
}
#endif

uint64_t foo(void) __attribute__((ifunc("foo_resolver")));
