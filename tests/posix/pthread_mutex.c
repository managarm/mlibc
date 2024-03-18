#include <pthread.h>
#include <assert.h>
#include <errno.h>

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

int main() {
	testAttr();
	testNormal();
	testRecursive();

	return 0;
}
