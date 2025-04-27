#ifndef _SYS_SHM_H
#define _SYS_SHM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/pid_t.h>
#include <abi-bits/shm.h>
#include <bits/size_t.h>
#include <time.h>

#include <sys/ipc.h>

#ifndef __MLIBC_ABI_ONLY

void *shmat(int __shmid, const void *__shmaddr, int __shmflg);
int shmctl(int __shmid, int __cmd, struct shmid_ds *__buf);
int shmdt(const void *__shmaddr);
int shmget(key_t __key, size_t __size, int __shmflg);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SHM_H */
