#ifndef _SYS_MSG_H
#define _SYS_MSG_H

#include <mlibc-config.h>

#include <abi-bits/msg.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_DEFAULT_SOURCE)
struct msginfo {
	int msgpool;
	int msgmap;
	int msgmax;
	int msgmnb;
	int msgmni;
	int msgssz;
	int msgtql;
	unsigned short int msgseg;
};
#endif /* defined(_DEFAULT_SOURCE) */

#ifndef __MLIBC_ABI_ONLY

int msgget(key_t __key, int __msgflg);

int msgctl(int __msqid, int __cmd, struct msqid_ds *__buf);

ssize_t msgrcv(int __msqid, void *__msgp, size_t __size, long __msgtyp, int __msgflg);
int msgsnd(int __msqid, const void *__msgp, size_t __size, int __msgflg);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MSG_H */
