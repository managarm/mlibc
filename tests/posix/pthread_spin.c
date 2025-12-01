#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <errno.h>
#include <stdio.h>

pthread_spinlock_t lock;
int variable = 0;

void *worker(void* arg) {
	(void)arg;

	int ret = pthread_spin_trylock(&lock);
	if (!variable) {
		assert(ret == EBUSY);
	} else {
		assert(ret == 0);
		variable = 0;
		pthread_spin_unlock(&lock);
	}

	return NULL;
}

int main() {
	assert(!pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE));
	assert(!pthread_spin_lock(&lock));

	/* glibc does not implement this optional POSIX behavior and hangs */
#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	assert(pthread_spin_lock(&lock) == EDEADLK);
#endif

	pthread_t thread;
	assert(!pthread_create(&thread, NULL, worker, NULL));
	pthread_join(thread, NULL);

	variable = 1;
	assert(!pthread_spin_unlock(&lock));
	assert(!pthread_create(&thread, NULL, worker, NULL));
	pthread_join(thread, NULL);

	assert(variable == 0);

	pthread_spin_destroy(&lock);
	return 0;
}
