#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

_Atomic int waiting, should_exit;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static void *worker(void *arg) {
	(void)arg;
	pthread_mutex_lock(&mtx);
	++waiting;
	while (!should_exit)
		pthread_cond_wait(&cond, &mtx);
	pthread_mutex_unlock(&mtx);
	return NULL;
}

static void test_broadcast_wakes_all() {
	pthread_t t1, t2;
	pthread_create(&t1, NULL, &worker, NULL);
	pthread_create(&t2, NULL, &worker, NULL);

	// Wait until the workers have actually entered the cond_wait
	// before doing a broadcast.
	while (waiting != 2 || pthread_mutex_trylock(&mtx) == EBUSY)
		usleep(150000); // 150ms

	should_exit = 1;
	assert(!pthread_cond_broadcast(&cond));
	pthread_mutex_unlock(&mtx);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}

static void test_timedwait_timedout() {
	struct timespec before_now;
	assert(!clock_gettime(CLOCK_REALTIME, &before_now));
	before_now.tv_nsec -= 10000;

	pthread_mutex_lock(&mtx);
	int e = pthread_cond_timedwait(&cond, &mtx, &before_now);
	assert(e == ETIMEDOUT);
	pthread_mutex_unlock(&mtx);

	long nanos_per_second = 1000000000;
	struct timespec after_now;
	assert(!clock_gettime(CLOCK_REALTIME, &after_now));
	after_now.tv_nsec += nanos_per_second / 10; // 100ms
	if (after_now.tv_nsec >= nanos_per_second) {
		after_now.tv_nsec -= nanos_per_second;
		after_now.tv_sec++;
	}

	pthread_mutex_lock(&mtx);
	e = pthread_cond_timedwait(&cond, &mtx, &after_now);
	assert(e == ETIMEDOUT);
	pthread_mutex_unlock(&mtx);

	after_now.tv_nsec += nanos_per_second;
	pthread_mutex_lock(&mtx);
	e = pthread_cond_timedwait(&cond, &mtx, &after_now);
	assert(e == EINVAL);
	pthread_mutex_unlock(&mtx);
}

int main() {
	test_broadcast_wakes_all();
	test_timedwait_timedout();
}
