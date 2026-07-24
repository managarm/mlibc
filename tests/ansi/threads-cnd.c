#include <assert.h>
#include <stddef.h>
#include <threads.h>
#include <time.h>

static mtx_t mutex;
static cnd_t cond;
static cnd_t ready_cond;
static int waiting = 0;
static int should_exit = 0;

static int worker_broadcast(void *arg) {
	(void)arg;
	int ret = mtx_lock(&mutex);
	assert(ret == thrd_success);

	waiting++;
	ret = cnd_signal(&ready_cond);
	assert(ret == thrd_success);

	while (!should_exit) {
		ret = cnd_wait(&cond, &mutex);
		assert(ret == thrd_success);
	}

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	return 0;
}

static void test_broadcast(void) {
	int ret = mtx_init(&mutex, mtx_plain);
	assert(ret == thrd_success);
	ret = cnd_init(&cond);
	assert(ret == thrd_success);
	ret = cnd_init(&ready_cond);
	assert(ret == thrd_success);

	waiting = 0;
	should_exit = 0;

	thrd_t t1, t2;
	ret = thrd_create(&t1, worker_broadcast, NULL);
	assert(ret == thrd_success);
	ret = thrd_create(&t2, worker_broadcast, NULL);
	assert(ret == thrd_success);

	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);
	while (waiting < 2) {
		ret = cnd_wait(&ready_cond, &mutex);
		assert(ret == thrd_success);
	}

	should_exit = 1;
	ret = cnd_broadcast(&cond);
	assert(ret == thrd_success);

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	int res1 = -1;
	ret = thrd_join(t1, &res1);
	assert(ret == thrd_success);
	assert(res1 == 0);

	int res2 = -1;
	ret = thrd_join(t2, &res2);
	assert(ret == thrd_success);
	assert(res2 == 0);

	cnd_destroy(&cond);
	cnd_destroy(&ready_cond);
	mtx_destroy(&mutex);
}

static int worker_signal(void *arg) {
	(void)arg;
	int ret = mtx_lock(&mutex);
	assert(ret == thrd_success);

	waiting = 1;
	ret = cnd_signal(&cond);
	assert(ret == thrd_success);

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	return 0;
}

static void test_signal(void) {
	int ret = mtx_init(&mutex, mtx_plain);
	assert(ret == thrd_success);
	ret = cnd_init(&cond);
	assert(ret == thrd_success);

	waiting = 0;

	thrd_t t;
	ret = thrd_create(&t, worker_signal, NULL);
	assert(ret == thrd_success);

	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);
	while (!waiting) {
		ret = cnd_wait(&cond, &mutex);
		assert(ret == thrd_success);
	}
	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	int res = -1;
	ret = thrd_join(t, &res);
	assert(ret == thrd_success);
	assert(res == 0);

	cnd_destroy(&cond);
	mtx_destroy(&mutex);
}

static void test_timedwait(void) {
	int ret = mtx_init(&mutex, mtx_plain);
	assert(ret == thrd_success);
	ret = cnd_init(&cond);
	assert(ret == thrd_success);

	struct timespec ts_past = {0, 0};
	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);

	ret = cnd_timedwait(&cond, &mutex, &ts_past);
	assert(ret == thrd_timedout);

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	struct timespec ts;
	int get_ret = timespec_get(&ts, TIME_UTC);
	assert(get_ret == TIME_UTC);
	ts.tv_nsec += 10000000;
	if (ts.tv_nsec >= 1000000000) {
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}

	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);

	ret = cnd_timedwait(&cond, &mutex, &ts);
	assert(ret == thrd_timedout);

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	struct timespec ts_invalid = {0, -1000};
	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);

	ret = cnd_timedwait(&cond, &mutex, &ts_invalid);
	assert(ret == thrd_error);

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	cnd_destroy(&cond);
	mtx_destroy(&mutex);
}

int main(void) {
	test_broadcast();
	test_signal();
	test_timedwait();
	return 0;
}
