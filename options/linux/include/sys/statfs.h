#ifndef _SYS_STATFS_H
#define _SYS_STATFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/statfs.h>

#ifndef __MLIBC_ABI_ONLY

int statfs(const char *, struct statfs *);
int fstatfs(int, struct statfs *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SYS_STATFS_H

