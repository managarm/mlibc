#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int main(void) {
	srand(time(NULL));

	int semid = -1;
	for (int i = 0; i < 10; i++) {
		key_t key = (key_t)rand();
		semid = semget(key, 1, 0644 | IPC_CREAT | IPC_EXCL);
		if (semid != -1) {
			break;
		}
	}
	assert(semid != -1);

	struct semid_ds buf = {0};
	union semun stat_arg;
	stat_arg.buf = &buf;
	int res_stat1 = semctl(semid, 0, IPC_STAT, stat_arg);
	assert(res_stat1 == 0);

	unsigned short nsems1 = buf.sem_nsems;
	assert(nsems1 == 1);

	unsigned int mode1 = buf.sem_perm.mode & 0777;
	assert(mode1 == 0644);

	time_t otime1 = buf.sem_otime;
	assert(otime1 == 0);

	time_t ctime1 = buf.sem_ctime;
	assert(ctime1 > 0);

	union semun arg;
	arg.val = 42;
	int res_setval = semctl(semid, 0, SETVAL, arg);
	assert(res_setval == 0);

	int val1 = semctl(semid, 0, GETVAL, arg);
	assert(val1 == 42);

	buf.sem_perm.mode = (buf.sem_perm.mode & ~0777) | 0600;
	int res_set = semctl(semid, 0, IPC_SET, stat_arg);
	assert(res_set == 0);

	struct semid_ds buf2 = {0};
	union semun stat_arg2;
	stat_arg2.buf = &buf2;
	int res_stat2 = semctl(semid, 0, IPC_STAT, stat_arg2);
	assert(res_stat2 == 0);

	unsigned int mode2 = buf2.sem_perm.mode & 0777;
	assert(mode2 == 0600);

	unsigned short nsems2 = buf2.sem_nsems;
	assert(nsems2 == 1);

	time_t ctime2 = buf2.sem_ctime;
	assert(ctime2 >= ctime1);

	int res_rm = semctl(semid, 0, IPC_RMID, arg);
	assert(res_rm == 0);

	return 0;
}
