#include <pthread.h>
#include <unistd.h>
#include <assert.h>

pthread_barrier_t barrier;
_Atomic int hitBarrierCount, pastBarrierCount;

static void *worker(void *arg) {
	(void)arg;
	hitBarrierCount++;
	pthread_barrier_wait(&barrier);
	pastBarrierCount++;
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
	int ret = pthread_create(&thread1, NULL, &worker, NULL);
	assert(!ret);

	pthread_t thread2;
	ret = pthread_create(&thread2, NULL, &worker, NULL);
	assert(!ret);

	sleep(1);

	// Make sure the barrier actually stops threads from proceeding.
	assert(pastBarrierCount == 0);
	assert(hitBarrierCount <= 2);

	hitBarrierCount++;
	pthread_barrier_wait(&barrier);
	assert(hitBarrierCount == 3);

	pthread_barrier_destroy(&barrier);

	return 0;
}
