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

#define SEM_STAT 18
#define SEM_INFO 19
#define SEM_STAT_ANY 20

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

struct seminfo {
	int semmap;
	int semmni;
	int semmns;
	int semmnu;
	int semmsl;
	int semopm;
	int semume;
	int semusz;
	int semvmx;
	int semaem;
};

#endif /* _ABIBITS_SEM_H */
