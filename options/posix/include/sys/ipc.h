#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#include <abi-bits/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

key_t ftok(const char *__path, int __proj_id);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif
