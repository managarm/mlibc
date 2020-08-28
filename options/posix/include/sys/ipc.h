#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <abi-bits/mode_t.h>

#define IPC_CREAT 01000
#define IPC_EXCL 02000
#define IPC_NOWAIT 04000

#define IPC_RMID 0
#define IPC_SET 1
#define IPC_STAT 2
#define IPC_INFO 3

#define IPC_PRIVATE ((key_t) 0)

typedef int key_t;

struct ipc_perm {
	key_t __ipc_perm_key;
	uid_t uid;
	gid_t gid;
	uid_t cuid;
	gid_t cgid;
	mode_t mode;
	int __ipc_perm_seq;
};

#endif
