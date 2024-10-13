#ifndef _SYS_AUXV_H
#define _SYS_AUXV_H

#define AT_NULL 0
#include <abi-bits/auxv.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

/* mlibc extension: Like getauxval but handles errors in a sane way. */
/* Success: Return 0. */
/* Failure: Return -1 and set errno. */
int peekauxval(unsigned long __type, unsigned long *__value);

unsigned long getauxval(unsigned long __type);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
