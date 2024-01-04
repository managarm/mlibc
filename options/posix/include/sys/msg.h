#ifndef _SYS_MSG_H
#define _SYS_MSG_H

#include <abi-bits/msg.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int msgget(key_t, int);

int msgctl(int msqid, int cmd, struct msqid_ds *buf);

ssize_t msgrcv(int, void *, size_t, long, int);
int msgsnd(int, const void *, size_t, int);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SYS_MSG_H
