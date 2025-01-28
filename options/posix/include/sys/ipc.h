#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <abi-bits/mode_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPC_CREAT 01000
#define IPC_EXCL 02000
#define IPC_NOWAIT 04000

#define IPC_RMID 0
#define IPC_SET 1
#define IPC_STAT 2
#define IPC_INFO 3

#define IPC_PRIVATE ((key_t) 0)

#if defined(__aarch64__) || defined(__i386__) || defined(__m68k__)
#define IPC_64 0x100
#elif defined(__x86_64__) || (defined(__riscv) && __riscv_xlen == 64)
#define IPC_64 0
#else
#error "Unsupported arch!"
#endif

typedef int key_t;

struct ipc64_perm {
	key_t __ipc_perm_key;
	uid_t uid;
	gid_t gid;
	uid_t cuid;
	gid_t cgid;
	mode_t mode;
	short __ipc_perm_seq;
	short __pad;
	unsigned long __unused[2];
};

#define ipc_perm ipc64_perm

#ifndef __MLIBC_ABI_ONLY

key_t ftok(const char *__path, int __proj_id);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif
