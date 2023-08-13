#ifndef _SYS_MSG_H
#define _SYS_MSG_H

#include <abi-bits/msg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int msgget(key_t, int);

int msgctl(int msqid, int cmd, struct msqid_ds *buf);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SYS_MSG_H
