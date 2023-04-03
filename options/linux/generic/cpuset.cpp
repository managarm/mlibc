#include <limits.h>
#include <sched.h>
#include <stdlib.h>

cpu_set_t *__mlibc_cpu_alloc(int num_cpus) {
	return reinterpret_cast<cpu_set_t *>(calloc(1, CPU_ALLOC_SIZE(num_cpus)));
}

#define CPU_MASK_BITS (CHAR_BIT * sizeof(__cpu_mask))

size_t __mlibc_cpu_alloc_size(int num_cpus) {
	/* calculate the (unaligned) remainder that doesn't neatly fit in one __cpu_mask; 0 or 1 */
	size_t remainder = ((num_cpus % CPU_MASK_BITS) + CPU_MASK_BITS - 1) / CPU_MASK_BITS;
	return sizeof(__cpu_mask) * (num_cpus / CPU_MASK_BITS + remainder);
}
