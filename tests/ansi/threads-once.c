#include <assert.h>
#include <stddef.h>
#include <threads.h>

static once_flag seq_flag = ONCE_FLAG_INIT;
static int seq_counter = 0;

static void seq_init_func(void) { seq_counter++; }

static void test_sequential(void) {
	call_once(&seq_flag, seq_init_func);
	assert(seq_counter == 1);

	call_once(&seq_flag, seq_init_func);
	assert(seq_counter == 1);
}

static once_flag con_flag = ONCE_FLAG_INIT;
static mtx_t mutex;
static cnd_t cond;
static int con_counter = 0;
static int func_entered = 0;
static int should_release = 0;

static void con_init_func(void) {
	int ret = mtx_lock(&mutex);
	assert(ret == thrd_success);

	con_counter++;
	func_entered = 1;
	ret = cnd_signal(&cond);
	assert(ret == thrd_success);

	while (!should_release) {
		ret = cnd_wait(&cond, &mutex);
		assert(ret == thrd_success);
	}

	ret = mtx_unlock(&mutex);
	assert(ret == thrd_success);
}

static int worker(void *arg) {
	(void)arg;
	call_once(&con_flag, con_init_func);
	return 0;
}

static void test_concurrent(void) {
	int ret = mtx_init(&mutex, mtx_plain);
	assert(ret == thrd_success);
	ret = cnd_init(&cond);
	assert(ret == thrd_success);

	thrd_t t1, t2;
	ret = thrd_create(&t1, worker, NULL);
	assert(ret == thrd_success);
	ret = thrd_create(&t2, worker, NULL);
	assert(ret == thrd_success);

	ret = mtx_lock(&mutex);
	assert(ret == thrd_success);
	while (!func_entered) {
		ret = cnd_wait(&cond, &mutex);
		assert(ret == thrd_success);
	}

	should_release = 1;
	ret = cnd_signal(&cond);
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

	call_once(&con_flag, con_init_func);

	assert(con_counter == 1);

	cnd_destroy(&cond);
	mtx_destroy(&mutex);
}

int main(void) {
	test_sequential();
	test_concurrent();
	return 0;
}
