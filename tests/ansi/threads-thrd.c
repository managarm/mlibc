#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <threads.h>
#include <time.h>

static mtx_t mutex;
static cnd_t cond;
static int ready = 0;
static thrd_t main_thread;
static thrd_t worker_thread;

static int worker_current_equal(void *arg) {
	(void)arg;
	worker_thread = thrd_current();

	int eq_self = thrd_equal(worker_thread, worker_thread);
	assert(eq_self != 0);

	int eq_main = thrd_equal(worker_thread, main_thread);
	assert(eq_main == 0);

	int ret = mtx_lock(&mutex);
	assert(ret == thrd_success);
	ready = 1;
	ret = cnd_signal(&cond);
	assert(ret == thrd_success);
	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	return 0;
}

static void test_current_equal_yield(void) {
	int ret = mtx_init(&mutex, mtx_plain);
	assert(ret == thrd_success);
	ret = cnd_init(&cond);
	assert(ret == thrd_success);

	main_thread = thrd_current();
	ready = 0;

	int eq_self = thrd_equal(main_thread, main_thread);
	assert(eq_self != 0);

	thrd_t t;
	ret = thrd_create(&t, worker_current_equal, NULL);
	assert(ret == thrd_success);

	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);
	while (!ready) {
		ret = cnd_wait(&cond, &mutex);
		assert(ret == thrd_success);
	}
	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	thrd_yield();

	int eq_diff = thrd_equal(main_thread, worker_thread);
	assert(eq_diff == 0);

	int res = -1;
	ret = thrd_join(t, &res);
	assert(ret == thrd_success);
	assert(res == 0);

	cnd_destroy(&cond);
	mtx_destroy(&mutex);
}

static void test_sleep(void) {
	struct timespec delay = {0, 10000000};
	int ret = thrd_sleep(&delay, NULL);
	assert(ret == 0);

	struct timespec delay_invalid = {0, -1000};
	ret = thrd_sleep(&delay_invalid, NULL);
	assert(ret < 0);
}

static int worker_exit(void *arg) {
	(void)arg;
	thrd_exit(INT_MAX);
}

static void test_exit_join(void) {
	thrd_t t;
	int ret = thrd_create(&t, worker_exit, NULL);
	assert(ret == thrd_success);

	int res = -1;
	ret = thrd_join(t, &res);
	assert(ret == thrd_success);
	assert(res == INT_MAX);
}

static int worker_detach(void *arg) {
	(void)arg;
	int ret = mtx_lock(&mutex);
	assert(ret == thrd_success);
	ready = 1;
	ret = cnd_signal(&cond);
	assert(ret == thrd_success);
	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	return 0;
}

static void test_detach(void) {
	int ret = mtx_init(&mutex, mtx_plain);
	assert(ret == thrd_success);
	ret = cnd_init(&cond);
	assert(ret == thrd_success);

	ready = 0;

	thrd_t t;
	ret = thrd_create(&t, worker_detach, NULL);
	assert(ret == thrd_success);

	ret = thrd_detach(t);
	assert(ret == thrd_success);

	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);
	while (!ready) {
		ret = cnd_wait(&cond, &mutex);
		assert(ret == thrd_success);
	}
	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	cnd_destroy(&cond);
	mtx_destroy(&mutex);
}

int main(void) {
	test_current_equal_yield();
	test_sleep();
	test_exit_join();
	test_detach();
	return 0;
}
