#ifndef _MLIBC_GLIBC_STDLIB_H
#define _MLIBC_GLIBC_STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>

typedef int (*comparison_fn_t) (const void *__a, const void *__b);

#ifndef __MLIBC_ABI_ONLY

#if defined(_DEFAULT_SOURCE)
int rpmatch(const char *__resp);
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_GLIBC_STDLIB_H */
