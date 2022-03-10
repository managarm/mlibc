#ifndef _SYS_FSUID_H
#define _SYS_FSUID_H

#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

int setfsuid(uid_t uid);
int setfsgid(gid_t gid);

#ifdef __cplusplus
}
#endif

#endif // _SYS_FSUID_H
