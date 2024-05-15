#ifndef _SYS_STATVFS_H
#define _SYS_STATVFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/statvfs.h>

#ifndef __MLIBC_ABI_ONLY

int statvfs(const char *__restrict, struct statvfs *__restrict);
int statvfs64(const char *__restrict, struct statvfs64 *__restrict);
int fstatvfs(int, struct statvfs *);
int fstatvfs64(int, struct statvfs64 *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SYS_STATVFS_H

