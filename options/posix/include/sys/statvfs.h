#ifndef _SYS_STATVFS_H
#define _SYS_STATVFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>

#include <abi-bits/statvfs.h>

#ifndef __MLIBC_ABI_ONLY

int statvfs(const char *__restrict __pathname, struct statvfs *__restrict __buf);
#if __MLIBC_LINUX_OPTION
int statvfs64(const char *__restrict __pathname, struct statvfs64 *__restrict __buf);
#endif /* !__MLIBC_LINUX_OPTION */
int fstatvfs(int __fd, struct statvfs *__buf);
#if __MLIBC_LINUX_OPTION
int fstatvfs64(int __fd, struct statvfs64 *__buf);
#endif /* !__MLIBC_LINUX_OPTION */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_STATVFS_H */

