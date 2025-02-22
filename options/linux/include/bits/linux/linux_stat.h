#ifndef _BITS_LINUX_STAT_H
#define _BITS_LINUX_STAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/statx.h>

#ifndef __MLIBC_ABI_ONLY

int statx(int __dirfd, const char *__pathname, int __flags, unsigned int __mask, struct statx *__statxbuf);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _BITS_LINUX_STAT_H */
