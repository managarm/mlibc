#include <semaphore.h>
#include <assert.h>
#include <errno.h>

int main() {
	sem_t sem;
	assert(sem_init(&sem, 0, 1) == 0);
	assert(sem_destroy(&sem) == 0);
	assert(sem_init(&sem, 0, -1) == -1);
	assert(errno == EINVAL);

	assert(sem_init(&sem, 0, 3) == 0);
	assert(sem_trywait(&sem) == 0);
	assert(sem_trywait(&sem) == 0);
	assert(sem_trywait(&sem) == 0);
	assert(sem_trywait(&sem) == -1);
	assert(errno == EAGAIN);
	assert(sem_destroy(&sem) == 0);

	sem_init(&sem, 0, 1);
	assert(sem_wait(&sem) == 0);
	assert(sem_trywait(&sem) == -1);
	assert(sem_post(&sem) == 0);
	assert(sem_wait(&sem) == 0);
	assert(sem_destroy(&sem) == 0);

	return 0;
}
