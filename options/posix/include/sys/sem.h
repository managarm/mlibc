#ifndef _SYS_SEM_H
#define _SYS_SEM_H

#include <bits/ansi/time_t.h>
#include <sys/ipc.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GETPID 11
#define GETVAL 12
#define GETALL 13
#define SETVAL 16
#define SETALL 17

#define SEM_UNDO 0x1000

struct sembuf {
	unsigned short int sem_num;
	short int sem_op;
	short int sem_flg;
};

struct semid_ds {
	struct ipc_perm sem_perm;
	time_t          sem_otime;
	time_t          sem_ctime;

	unsigned long   sem_nsems;
};

#ifndef __MLIBC_ABI_ONLY

int semget(key_t __key, int __nsems, int __semflg);
int semop(int __semid, struct sembuf *__sops, size_t __nsops);
int semctl(int __semid, int __semnum, int __op, ...);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SEM_H */
