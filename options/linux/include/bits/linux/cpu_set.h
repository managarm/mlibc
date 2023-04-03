#ifndef _LINUX_CPU_SET_H
#define _LINUX_CPU_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/cpu_set.h>
#include <bits/size_t.h>
#include <limits.h>
#include <stdlib.h>

cpu_set_t *__mlibc_cpu_alloc(int num_cpus);
size_t __mlibc_cpu_alloc_size(int num_cpus);

#define CPU_ALLOC_SIZE(n) __mlibc_cpu_alloc_size((n))
#define CPU_ALLOC(n) __mlibc_cpu_alloc((n))
#define CPU_FREE(set) free((set))

#ifdef __cplusplus
}
#endif

#endif /* _LINUX_CPU_SET_H */
