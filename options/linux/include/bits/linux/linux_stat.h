#ifndef _LINUX_STAT_H
#define _LINUX_STAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/statx.h>

#ifndef __MLIBC_ABI_ONLY

int statx(int, const char *, int, unsigned int, struct statx *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif
