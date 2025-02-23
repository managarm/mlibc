#include <sys/shm.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int main(void) {
    size_t page_size = getpagesize();

    int shmid = shmget(23123123, page_size, 0644 | IPC_CREAT | IPC_EXCL);
    assert(shmid != -1);

    struct shmid_ds buf;
    assert(shmctl(shmid, IPC_STAT, &buf) >= 0);
    assert((size_t)buf.shm_segsz == page_size);
    assert(buf.shm_cpid == getpid());

    void *attach_addr = shmat(shmid, NULL, SHM_RDONLY);
    assert(attach_addr != NULL);

    assert(shmctl(shmid, IPC_RMID, &buf) >= 0);
    assert(shmdt(attach_addr) == 0);
}
