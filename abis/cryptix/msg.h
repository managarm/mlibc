#ifndef _ABIBITS_MSG_H
#define _ABIBITS_MSG_H

#include <sys/ipc.h>
#include <bits/ansi/time_t.h>
#include <bits/types.h>
#include <abi-bits/pid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long msglen_t;
typedef unsigned long msgqnum_t;

struct msqid64_ds {
	struct ipc64_perm msg_perm;
#if (UINTPTR_MAX == UINT64_MAX) /* || x32 ABI */
	time_t msg_stime;
	time_t msg_rtime;
	time_t msg_ctime;
#else
	unsigned long msg_stime;
	unsigned long msg_stime_high;
	unsigned long msg_rtime;
	unsigned long msg_rtime_high;
	unsigned long msg_ctime;
	unsigned long msg_ctime_high;
#endif
	unsigned long msg_cbytes;
	msgqnum_t msg_qnum;
	msglen_t msg_qbytes;
	pid_t msg_lspid;
	pid_t msg_lrpid;
	unsigned long __unused[2];
};

#define msqid_ds msqid64_ds

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_MSG_H */

