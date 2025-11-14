
#ifndef _MLIBC_BSD_STDLIB_H
#define _MLIBC_BSD_STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>

#ifndef __MLIBC_ABI_ONLY

#if defined(_DEFAULT_SOURCE)
int getloadavg(double *__loadavg, int __count);
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_BSD_STDLIB_H */

