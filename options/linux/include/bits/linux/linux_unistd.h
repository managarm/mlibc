#ifndef _BITS_LINUX_UNISTD_H
#define _BITS_LINUX_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>

#ifndef __MLIBC_ABI_ONLY

#if defined(_DEFAULT_SOURCE)
int vhangup(void);
#endif

#if defined(_GNU_SOURCE)
int syncfs(int __fd);
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _BITS_LINUX_UNISTD_H */
