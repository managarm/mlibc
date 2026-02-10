#include <assert.h>
#include <pthread.h>
#include <stddef.h>

#define NUM_THREADS 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t status_cond = PTHREAD_COND_INITIALIZER;
int threads_waiting = 0;
int threads_woken = 0;

void *worker(void *arg) {
	(void) arg;
	assert(pthread_mutex_lock(&mutex) == 0);
	threads_waiting++;
	assert(pthread_cond_signal(&status_cond) == 0);
	assert(pthread_cond_wait(&cond, &mutex) == 0);
	threads_woken++;
	assert(pthread_cond_signal(&status_cond) == 0);
	assert(pthread_mutex_unlock(&mutex) == 0);
	return NULL;
}

int main() {
	pthread_t p[NUM_THREADS];

	for (size_t i = 0; i < NUM_THREADS; i++) {
		assert(pthread_create(&p[i], NULL, worker, NULL) == 0);
	}

	assert(pthread_mutex_lock(&mutex) == 0);
	while (threads_waiting < NUM_THREADS) {
		assert(pthread_cond_wait(&status_cond, &mutex) == 0);
	}

	assert(threads_woken == 0);
	assert(pthread_cond_signal(&cond) == 0);

	while (threads_woken < 1) {
		assert(pthread_cond_wait(&status_cond, &mutex) == 0);
	}
	assert(threads_woken >= 1);

	assert(pthread_cond_broadcast(&cond) == 0);

	while (threads_woken < NUM_THREADS) {
		assert(pthread_cond_wait(&status_cond, &mutex) == 0);
	}
	assert(pthread_mutex_unlock(&mutex) == 0);

	for (size_t i = 0; i < NUM_THREADS; i++) {
		assert(pthread_join(p[i], NULL) == 0);
	}

	assert(threads_woken == NUM_THREADS);

	return 0;
}
