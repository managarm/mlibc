#ifndef _MLIBC_INTERNAL_CPU_SET_H
#define _MLIBC_INTERNAL_CPU_SET_H

typedef unsigned long __cpu_mask;

#define __CPU_SETSIZE 1024
#define __NCPUBITS (8 * sizeof(__cpu_mask))

typedef struct {
	__cpu_mask __bits[__CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

#endif /* _MLIBC_INTERNAL_CPU_SET_H */
