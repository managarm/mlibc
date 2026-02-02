#ifndef _SYS_STATVFS_H
#define _SYS_STATVFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>

#include <abi-bits/statvfs.h>

#ifndef __MLIBC_ABI_ONLY

#if __MLIBC_BSD_OPTION
/* This corresponds to the NetBSD version of getmntinfo.
   FreeBSD and OpenBSD are known to have a different declaration (in a different header file). */
int getmntinfo(struct statvfs **__mntbufp, int __mode);
#endif

int statvfs(const char *__restrict __pathname, struct statvfs *__restrict __buf);
int fstatvfs(int __fd, struct statvfs *__buf);

#if __MLIBC_LINUX_OPTION && defined(_LARGEFILE64_SOURCE)
int statvfs64(const char *__restrict __pathname, struct statvfs64 *__restrict __buf);
int fstatvfs64(int __fd, struct statvfs64 *__buf);
#endif /* __MLIBC_LINUX_OPTION && defined(_LARGEFILE64_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_STATVFS_H */
