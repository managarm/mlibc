#ifndef _BITS_LINUX_CPU_SET_H
#define _BITS_LINUX_CPU_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/cpu_set.h>
#include <bits/size_t.h>
#include <limits.h>
#include <stdlib.h>

#ifndef __MLIBC_ABI_ONLY

cpu_set_t *__mlibc_cpu_alloc(int __num_cpus);
size_t __mlibc_cpu_alloc_size(int __num_cpus);

void __mlibc_cpu_zero(const size_t __setsize, cpu_set_t *__set);
void __mlibc_cpu_set(const int __cpu, const size_t __setsize, cpu_set_t *__set);
void __mlibc_cpu_clear(const int __cpu, const size_t __setsize, cpu_set_t *__set);
int __mlibc_cpu_isset(const int __cpu, const size_t __setsize, const cpu_set_t *__set);
int __mlibc_cpu_count(const size_t __setsize, const cpu_set_t *__set);

#define CPU_ALLOC_SIZE(n) __mlibc_cpu_alloc_size((n))
#define CPU_ALLOC(n) __mlibc_cpu_alloc((n))
#define CPU_FREE(set) free((set))

#define CPU_ZERO_S(setsize, set) __mlibc_cpu_zero((setsize), (set))
#define CPU_ZERO(set) CPU_ZERO_S(sizeof(cpu_set_t), set)

#define CPU_SET_S(cpu, setsize, set) __mlibc_cpu_set((cpu), (setsize), (set))
#define CPU_SET(cpu, set) CPU_SET_S(cpu, sizeof(cpu_set_t), set)

#define CPU_CLR_S(cpu, setsize, set) __mlibc_cpu_clear((cpu), (setsize), (set))
#define CPU_CLR(cpu, set) CPU_CLR_S(cpu, sizeof(cpu_set_t), set)

#define CPU_ISSET_S(cpu, setsize, set) __mlibc_cpu_isset((cpu), (setsize), (set))
#define CPU_ISSET(cpu, set) CPU_ISSET_S(cpu, sizeof(cpu_set_t), set)

#define CPU_COUNT_S(setsize, set) __mlibc_cpu_count((setsize), (set))
#define CPU_COUNT(set) CPU_COUNT_S(sizeof(cpu_set_t), set)

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _BITS_LINUX_CPU_SET_H */
