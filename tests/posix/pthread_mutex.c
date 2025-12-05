#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define TEST_ATTR(attr, field, value) ({ \
		int x; \
		assert(!pthread_mutexattr_set ## field (&(attr), (value))); \
		assert(!pthread_mutexattr_get ## field (&(attr), &x)); \
		assert(x == (value)); \
	})

int variable;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void *worker(void *arg) {
	(void)arg;
	pthread_mutex_lock(&mutex);
	variable = 1;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

static void testAttr() {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);

	TEST_ATTR(attr, type, PTHREAD_MUTEX_DEFAULT);
	TEST_ATTR(attr, type, PTHREAD_MUTEX_NORMAL);
	TEST_ATTR(attr, type, PTHREAD_MUTEX_ERRORCHECK);
	TEST_ATTR(attr, type, PTHREAD_MUTEX_RECURSIVE);

	TEST_ATTR(attr, robust, PTHREAD_MUTEX_STALLED);
	TEST_ATTR(attr, robust, PTHREAD_MUTEX_ROBUST);

	TEST_ATTR(attr, protocol, PTHREAD_PRIO_NONE);
	TEST_ATTR(attr, protocol, PTHREAD_PRIO_INHERIT);
	TEST_ATTR(attr, protocol, PTHREAD_PRIO_PROTECT);

	TEST_ATTR(attr, pshared, PTHREAD_PROCESS_PRIVATE);
	TEST_ATTR(attr, pshared, PTHREAD_PROCESS_SHARED);

	// TODO: sched_get_priority* is unimplemented.
	// int prio = sched_get_priority_max(SCHED_FIFO);
	// TEST_ATTR(attr, prioceiling, prio);

	pthread_mutexattr_destroy(&attr);
}

static void testNormal() {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);

	pthread_mutex_lock(&mutex);
	variable = 0;

	pthread_t thread;
	int ret = pthread_create(&thread, NULL, &worker, NULL);
	assert(!ret);

	assert(pthread_mutex_trylock(&mutex) == EBUSY);
	pthread_mutex_unlock(&mutex);

	ret = pthread_join(thread, NULL);
	assert(!ret);
	assert(variable == 1);

	pthread_mutex_destroy(&mutex);
}

static void testRecursive() {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex, &attr);
	pthread_mutexattr_destroy(&attr);

	pthread_mutex_lock(&mutex);
	variable = 0;

	pthread_t thread;
	int ret = pthread_create(&thread, NULL, &worker, NULL);
	assert(!ret);

	assert(pthread_mutex_trylock(&mutex) == 0);
	pthread_mutex_unlock(&mutex);
	pthread_mutex_unlock(&mutex);

	ret = pthread_join(thread, NULL);
	assert(!ret);
	assert(variable == 1);

	pthread_mutex_destroy(&mutex);
}

pthread_mutex_t timedMutex;
pthread_barrier_t timedMutexBarrier;

void *testTimedLockWorker(void *arg) {
	(void) arg;

	pthread_mutex_lock(&timedMutex);
	pthread_barrier_wait(&timedMutexBarrier);
	usleep(1500000);
	pthread_mutex_unlock(&timedMutex);

	return NULL;
}

static void testTimedLock() {
	pthread_t test_thread;

	int ret = pthread_barrier_init(&timedMutexBarrier, NULL, 2);
	assert(ret == 0);
	ret = pthread_mutex_init(&timedMutex, NULL);
	assert(ret == 0);

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 1;

	ret = pthread_mutex_timedlock(&timedMutex, &ts);
	assert(ret == 0);

	ret = pthread_mutex_unlock(&timedMutex);
	assert(ret == 0);

	ret = pthread_create(&test_thread, NULL, testTimedLockWorker, NULL);
	if (ret != 0) {
		fprintf(stderr, "pthread_create: %s\n", strerror(ret));
		exit(1);
	}

	pthread_barrier_wait(&timedMutexBarrier);

	struct timespec startTime;
	clock_gettime(CLOCK_REALTIME, &startTime);
	ts = startTime;
	ts.tv_sec += 1;
	ret = pthread_mutex_timedlock(&timedMutex, &ts);
	assert(ret == ETIMEDOUT);

	struct timespec endTime;
	clock_gettime(CLOCK_REALTIME, &endTime);

	struct timespec diff;
	diff.tv_sec = endTime.tv_sec - startTime.tv_sec;
	diff.tv_nsec = endTime.tv_nsec - startTime.tv_nsec;
	if (diff.tv_nsec < 0) {
		diff.tv_sec -= 1;
		diff.tv_nsec += 1000000000;
	}

	assert(diff.tv_sec >= 1);

	ret = pthread_join(test_thread, NULL);
	assert(ret == 0);

	pthread_barrier_destroy(&timedMutexBarrier);
	pthread_mutex_destroy(&timedMutex);
}

int main() {
	testAttr();
	testNormal();
	testRecursive();
	testTimedLock();

	return 0;
}
