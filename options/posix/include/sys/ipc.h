#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#define IPC_CREAT 01000
#define IPC_EXCL 02000
#define IPC_NOWAIT 04000

#define IPC_RMID 0
#define IPC_SET 1
#define IPC_STAT 2
#define IPC_INFO 3

#define IPC_PRIVATE ((key_t) 0)

typedef int key_t;

#endif
