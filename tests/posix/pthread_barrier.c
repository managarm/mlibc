#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define THREAD_COUNT 4

pthread_barrier_t barrier;
int hitBarrierCount = 0;
int pastBarrierCount = 0;

static void *worker(void *arg) {
	int iterations = (uintptr_t)arg;

	for (int i = 0; i < iterations; i++) {
		__atomic_fetch_add(&hitBarrierCount, 1, __ATOMIC_RELAXED);
		pthread_barrier_wait(&barrier);
		__atomic_fetch_add(&pastBarrierCount, 1, __ATOMIC_RELAXED);
	}
	return NULL;
}

int main() {
	// pthread_barrierattr_t
	pthread_barrierattr_t attr;
	pthread_barrierattr_init(&attr);

	int pshared;
	pthread_barrierattr_getpshared(&attr, &pshared);
	assert(pshared == PTHREAD_PROCESS_PRIVATE);

	pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_barrierattr_getpshared(&attr, &pshared);
	assert(pshared == PTHREAD_PROCESS_SHARED);

	pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
	pthread_barrierattr_getpshared(&attr, &pshared);
	assert(pshared == PTHREAD_PROCESS_PRIVATE);

	// pthread_barrier_t
	pthread_barrier_init(&barrier, &attr, 3);
	pthread_barrierattr_destroy(&attr);

	pthread_t thread1;
	int ret = pthread_create(&thread1, NULL, &worker, (void *)1);
	assert(!ret);

	pthread_t thread2;
	ret = pthread_create(&thread2, NULL, &worker, (void *)1);
	assert(!ret);

	sleep(1);

	// Make sure the barrier actually stops threads from proceeding.
	assert(__atomic_load_n(&pastBarrierCount, __ATOMIC_RELAXED) == 0);
	assert(__atomic_load_n(&hitBarrierCount, __ATOMIC_RELAXED) <= 2);

	__atomic_fetch_add(&hitBarrierCount, 1, __ATOMIC_RELAXED);
	pthread_barrier_wait(&barrier);
	__atomic_fetch_add(&pastBarrierCount, 1, __ATOMIC_RELAXED);
	assert(__atomic_load_n(&hitBarrierCount, __ATOMIC_RELAXED) == 3);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	assert(__atomic_load_n(&pastBarrierCount, __ATOMIC_RELAXED) == 3);

	pthread_barrier_destroy(&barrier);

	pthread_barrier_init(&barrier, NULL, THREAD_COUNT);
	__atomic_store_n(&hitBarrierCount, 0, __ATOMIC_RELAXED);
	__atomic_store_n(&pastBarrierCount, 0, __ATOMIC_RELAXED);

	pthread_t threads[THREAD_COUNT];
	for (int i = 0; i < THREAD_COUNT; i++) {
		ret = pthread_create(&threads[i], NULL, &worker, (void *)1000);
		assert(!ret);
	}

	for (int i = 0; i < THREAD_COUNT; i++) {
		pthread_join(threads[i], NULL);
	}

	assert(__atomic_load_n(&hitBarrierCount, __ATOMIC_RELAXED) == THREAD_COUNT * 1000);
	assert(__atomic_load_n(&pastBarrierCount, __ATOMIC_RELAXED) == THREAD_COUNT * 1000);

	pthread_barrier_destroy(&barrier);

	return 0;
}
