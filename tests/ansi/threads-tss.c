#include <assert.h>
#include <stddef.h>
#include <threads.h>

static tss_t key_seq;
static tss_t key_con;
static int dtor_called = 0;

static void test_sequential(void) {
	int ret = tss_create(&key_seq, NULL);
	assert(ret == thrd_success);

	void *val = tss_get(key_seq);
	assert(val == NULL);

	ret = tss_set(key_seq, &key_seq);
	assert(ret == thrd_success);

	val = tss_get(key_seq);
	assert(val == &key_seq);

	tss_delete(key_seq);
}

static int worker(void *arg) {
	(void)arg;

	void *val = tss_get(key_con);
	assert(val == NULL);

	int ret = tss_set(key_con, &key_con);
	assert(ret == thrd_success);

	val = tss_get(key_con);
	assert(val == &key_con);

	return 0;
}

static void dtor_func(void *val) {
	assert(val != NULL);
	dtor_called++;
}

static void test_concurrent(void) {
	int ret = tss_create(&key_con, dtor_func);
	assert(ret == thrd_success);

	thrd_t t;
	ret = thrd_create(&t, worker, NULL);
	assert(ret == thrd_success);

	int res = -1;
	ret = thrd_join(t, &res);
	assert(ret == thrd_success);
	assert(res == 0);

	assert(dtor_called == 1);

	void *val = tss_get(key_con);
	assert(val == NULL);

	tss_delete(key_con);
}

int main(void) {
	test_sequential();
	test_concurrent();
	return 0;
}
