#ifndef _SYS_SYSMACROS_H
#define _SYS_SYSMACROS_H

#include <bits/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define major(__dev) ((__mlibc_uint32)(__dev))
#define minor(__dev) ((__mlibc_uint32)((__dev) >> 32))
#define makedev(__major, __minor) ((__mlibc_uint64)(__major) | ((__mlibc_uint64)(__minor) << 32))

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SYSMACROS_H */
