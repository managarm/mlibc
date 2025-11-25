#ifndef _BITS_LINUX_STAT_H
#define _BITS_LINUX_STAT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _GNU_SOURCE
#include <abi-bits/statx.h>
#endif

#ifndef __MLIBC_ABI_ONLY

#ifdef _GNU_SOURCE
int statx(int __dirfd, const char *__pathname, int __flags, unsigned int __mask, struct statx *__statxbuf);
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _BITS_LINUX_STAT_H */
