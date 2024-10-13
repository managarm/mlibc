#ifndef _SYS_FSUID_H
#define _SYS_FSUID_H

#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int setfsuid(uid_t __uid);
int setfsgid(gid_t __gid);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_FSUID_H */
