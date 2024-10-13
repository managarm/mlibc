#ifndef _SYS_STATFS_H
#define _SYS_STATFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/statfs.h>

#ifndef __MLIBC_ABI_ONLY

int statfs(const char *__path, struct statfs *__buf);
int fstatfs(int __fd, struct statfs *__buf);
int fstatfs64(int __fd, struct statfs64 *__buf);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_STATFS_H */

