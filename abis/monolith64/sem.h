#ifndef _ABIBITS_SEM_H
#define _ABIBITS_SEM_H

#include <abi-bits/time.h>
#include <abi-bits/ipc.h>

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
	unsigned long   __unused[2];
};

#endif /* _ABIBITS_SEM_H */
