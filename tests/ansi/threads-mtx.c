#include <assert.h>
#include <stddef.h>
#include <threads.h>
#include <time.h>

static mtx_t mutex;
static int shared_var = 0;

static int worker_plain(void *arg) {
	(void)arg;
	int ret = mtx_lock(&mutex);
	assert(ret == thrd_success);

	shared_var = 42;

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	return 0;
}

static void test_plain(void) {
	int ret = mtx_init(&mutex, mtx_plain);
	assert(ret == thrd_success);

	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);

	shared_var = 0;

	thrd_t thread;
	ret = thrd_create(&thread, worker_plain, NULL);
	assert(ret == thrd_success);

	ret = mtx_trylock(&mutex);
	assert(ret == thrd_busy);

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	int res = -1;
	ret = thrd_join(thread, &res);
	assert(ret == thrd_success);
	assert(res == 0);
	assert(shared_var == 42);

	mtx_destroy(&mutex);
}

static mtx_t rec_mutex;
static mtx_t rec_state_mutex;
static cnd_t rec_cond;
static int rec_ready = 0;
static int rec_should_exit = 0;

static int worker_recursive(void *arg) {
	(void)arg;
	int ret = mtx_lock(&rec_mutex);
	assert(ret == thrd_success);

	ret = mtx_lock(&rec_state_mutex);
	assert(ret == thrd_success);
	rec_ready = 1;
	ret = cnd_signal(&rec_cond);
	assert(ret == thrd_success);

	while (!rec_should_exit) {
		ret = cnd_wait(&rec_cond, &rec_state_mutex);
		assert(ret == thrd_success);
	}

	ret = mtx_unlock(&rec_state_mutex);
	assert(ret == thrd_success);

	ret = mtx_unlock(&rec_mutex);
	assert(ret == thrd_success);

	return 0;
}

static void test_recursive(void) {
	int ret = mtx_init(&mutex, mtx_recursive);
	assert(ret == thrd_success);

	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);

	ret = mtx_trylock(&mutex);
	assert(ret == thrd_success);

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);

	mtx_destroy(&mutex);

	ret = mtx_init(&rec_mutex, mtx_recursive);
	assert(ret == thrd_success);
	ret = mtx_init(&rec_state_mutex, mtx_plain);
	assert(ret == thrd_success);
	ret = cnd_init(&rec_cond);
	assert(ret == thrd_success);

	rec_ready = 0;
	rec_should_exit = 0;

	thrd_t thread;
	ret = thrd_create(&thread, worker_recursive, NULL);
	assert(ret == thrd_success);

	ret = mtx_lock(&rec_state_mutex);
	assert(ret == thrd_success);
	while (!rec_ready) {
		ret = cnd_wait(&rec_cond, &rec_state_mutex);
		assert(ret == thrd_success);
	}
	ret = mtx_unlock(&rec_state_mutex);
	assert(ret == thrd_success);

	ret = mtx_trylock(&rec_mutex);
	assert(ret == thrd_busy);

	ret = mtx_lock(&rec_state_mutex);
	assert(ret == thrd_success);
	rec_should_exit = 1;
	ret = cnd_signal(&rec_cond);
	assert(ret == thrd_success);
	ret = mtx_unlock(&rec_state_mutex);
	assert(ret == thrd_success);

	int res = -1;
	ret = thrd_join(thread, &res);
	assert(ret == thrd_success);
	assert(res == 0);

	mtx_destroy(&rec_mutex);
	mtx_destroy(&rec_state_mutex);
	cnd_destroy(&rec_cond);
}

static mtx_t timed_mutex;
static mtx_t state_mutex;
static cnd_t cond;
static int ready = 0;
static int should_exit = 0;

static int worker_timed(void *arg) {
	(void)arg;
	int ret = mtx_lock(&timed_mutex);
	assert(ret == thrd_success);

	ret = mtx_lock(&state_mutex);
	assert(ret == thrd_success);
	ready = 1;
	ret = cnd_signal(&cond);
	assert(ret == thrd_success);

	while (!should_exit) {
		ret = cnd_wait(&cond, &state_mutex);
		assert(ret == thrd_success);
	}

	ret = mtx_unlock(&state_mutex);
	assert(ret == thrd_success);

	ret = mtx_unlock(&timed_mutex);
	assert(ret == thrd_success);

	return 0;
}

static void test_timed(void) {
	int ret = mtx_init(&timed_mutex, mtx_timed);
	assert(ret == thrd_success);

	struct timespec ts_past = {0, 0};
	ret = mtx_timedlock(&timed_mutex, &ts_past);
	assert(ret == thrd_success);

	ret = mtx_unlock(&timed_mutex);
	assert(ret == thrd_success);

	ret = mtx_init(&state_mutex, mtx_plain);
	assert(ret == thrd_success);

	ret = cnd_init(&cond);
	assert(ret == thrd_success);

	ready = 0;
	should_exit = 0;

	thrd_t thread;
	ret = thrd_create(&thread, worker_timed, NULL);
	assert(ret == thrd_success);

	ret = mtx_lock(&state_mutex);
	assert(ret == thrd_success);
	while (!ready) {
		ret = cnd_wait(&cond, &state_mutex);
		assert(ret == thrd_success);
	}
	ret = mtx_unlock(&state_mutex);
	assert(ret == thrd_success);

	ret = mtx_timedlock(&timed_mutex, &ts_past);
	assert(ret == thrd_timedout);

	struct timespec ts;
	int get_ret = timespec_get(&ts, TIME_UTC);
	assert(get_ret == TIME_UTC);
	ts.tv_nsec += 10000000;
	if (ts.tv_nsec >= 1000000000) {
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}

	ret = mtx_timedlock(&timed_mutex, &ts);
	assert(ret == thrd_timedout);

	ret = mtx_lock(&state_mutex);
	assert(ret == thrd_success);
	should_exit = 1;
	ret = cnd_signal(&cond);
	assert(ret == thrd_success);
	ret = mtx_unlock(&state_mutex);
	assert(ret == thrd_success);

	int res = -1;
	ret = thrd_join(thread, &res);
	assert(ret == thrd_success);
	assert(res == 0);

	mtx_destroy(&timed_mutex);
	mtx_destroy(&state_mutex);
	cnd_destroy(&cond);
}

int main(void) {
	test_plain();
	test_recursive();
	test_timed();
	return 0;
}
