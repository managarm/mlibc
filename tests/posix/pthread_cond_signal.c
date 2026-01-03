#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define NUM_THREADS 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int threads_waiting = 0;
int threads_woken = 0;

void *worker(void *arg) {
	(void) arg;
	assert(pthread_mutex_lock(&mutex) == 0);
	threads_waiting++;
	assert(pthread_cond_wait(&cond, &mutex) == 0);
	threads_woken++;
	assert(pthread_mutex_unlock(&mutex) == 0);
	return NULL;
}

int main() {
	pthread_t p[NUM_THREADS];

	for (size_t i = 0; i < NUM_THREADS; i++) {
		assert(pthread_create(&p[i], NULL, worker, NULL) == 0);
	}

	while (true) {
		assert(pthread_mutex_lock(&mutex) == 0);
		if (threads_waiting == NUM_THREADS) {
			assert(pthread_mutex_unlock(&mutex) == 0);
			break;
		}
		assert(pthread_mutex_unlock(&mutex) == 0);
		usleep(1000);
	}
	usleep(10000);

	assert(pthread_mutex_lock(&mutex) == 0);
	assert(pthread_cond_signal(&cond) == 0);
	assert(pthread_mutex_unlock(&mutex) == 0);

	usleep(10000);

	assert(pthread_mutex_lock(&mutex) == 0);
	assert(threads_woken == 1);
	assert(pthread_mutex_unlock(&mutex) == 0);

	assert(pthread_mutex_lock(&mutex) == 0);
	assert(pthread_cond_broadcast(&cond) == 0);
	assert(pthread_mutex_unlock(&mutex) == 0);

	for (size_t i = 0; i < NUM_THREADS; i++) {
		assert(pthread_join(p[i], NULL) == 0);
	}

	assert(threads_woken == NUM_THREADS);

	return 0;
}
