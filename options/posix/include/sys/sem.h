#ifndef _SYS_SEM_H
#define _SYS_SEM_H

#include <abi-bits/sem.h>
#include <bits/ansi/time_t.h>
#include <sys/ipc.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int semget(key_t __key, int __nsems, int __semflg);
int semop(int __semid, struct sembuf *__sops, size_t __nsops);
int semctl(int __semid, int __semnum, int __op, ...);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SEM_H */
