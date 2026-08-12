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

/* WARNING: this diverges from the kernel ABI for 32-bit architectures;
 * the kernel splits the time_t fields into low/high fields for 32-bit,
 * which is an alignment and endianness concern. */
struct msqid_ds {
	struct ipc_perm msg_perm;
	time_t msg_stime;
	time_t msg_rtime;
	time_t msg_ctime;
	unsigned long msg_cbytes;
	msgqnum_t msg_qnum;
	msglen_t msg_qbytes;
	pid_t msg_lspid;
	pid_t msg_lrpid;
	unsigned long __unused[2];
};

#define msqid64_ds msqid_ds

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_MSG_H */

