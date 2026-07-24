#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

int main(void) {
	size_t page_size = getpagesize();

	srand(time(NULL));

	int shmid = -1;
	for (int i = 0; i < 10; i++) {
		key_t key = (key_t)rand();
		shmid = shmget(key, page_size, 0644 | IPC_CREAT | IPC_EXCL);
		if (shmid != -1) {
			break;
		}
	}
	assert(shmid != -1);

	struct shmid_ds buf = {0};
	int stat_res1 = shmctl(shmid, IPC_STAT, &buf);
	assert(stat_res1 >= 0);

	size_t seg_sz1 = buf.shm_segsz;
	assert(seg_sz1 == page_size);

	pid_t cpid1 = buf.shm_cpid;
	pid_t my_pid = getpid();
	assert(cpid1 == my_pid);

	pid_t lpid1 = buf.shm_lpid;
	assert(lpid1 == 0);

	unsigned long nattch1 = buf.shm_nattch;
	assert(nattch1 == 0);

	time_t atime1 = buf.shm_atime;
	assert(atime1 == 0);

	time_t dtime1 = buf.shm_dtime;
	assert(dtime1 == 0);

	time_t ctime1 = buf.shm_ctime;
	assert(ctime1 > 0);

	unsigned int mode1 = buf.shm_perm.mode & 0777;
	assert(mode1 == 0644);

	void *attach_addr = shmat(shmid, NULL, SHM_RDONLY);
	assert(attach_addr != (void *)-1);

	int stat_res2 = shmctl(shmid, IPC_STAT, &buf);
	assert(stat_res2 >= 0);

	unsigned long nattch2 = buf.shm_nattch;
	assert(nattch2 == 1);

	pid_t lpid2 = buf.shm_lpid;
	assert(lpid2 == my_pid);

	time_t atime2 = buf.shm_atime;
	assert(atime2 > 0);

	buf.shm_perm.mode = (buf.shm_perm.mode & ~0777) | 0600;
	int set_res = shmctl(shmid, IPC_SET, &buf);
	assert(set_res >= 0);

	struct shmid_ds buf2 = {0};
	int stat_res3 = shmctl(shmid, IPC_STAT, &buf2);
	assert(stat_res3 >= 0);

	unsigned int mode2 = buf2.shm_perm.mode & 0777;
	assert(mode2 == 0600);

	time_t ctime2 = buf2.shm_ctime;
	assert(ctime2 >= ctime1);

	int dt_res = shmdt(attach_addr);
	assert(dt_res == 0);

	int stat_res4 = shmctl(shmid, IPC_STAT, &buf);
	assert(stat_res4 >= 0);

	unsigned long nattch4 = buf.shm_nattch;
	assert(nattch4 == 0);

	time_t dtime4 = buf.shm_dtime;
	assert(dtime4 > 0);

	int rmid_res = shmctl(shmid, IPC_RMID, NULL);
	assert(rmid_res >= 0);

	return 0;
}
