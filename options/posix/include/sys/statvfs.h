#ifndef _SYS_STATVFS_H
#define _SYS_STATVFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/statvfs.h>

int statvfs(const char *__restrict, struct statvfs *__restrict);
int fstatvfs(int, struct statvfs *);

#ifdef __cplusplus
}
#endif

#endif // _SYS_STATVFS_H

