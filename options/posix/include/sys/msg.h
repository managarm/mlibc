#ifndef _SYS_MSG_H
#define _SYS_MSG_H

#include <sys/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

int msgget(key_t, int);

int msgctl(int msqid, int cmd, struct msqid_ds *buf);

#ifdef __cplusplus
}
#endif

#endif // _SYS_MSG_H
