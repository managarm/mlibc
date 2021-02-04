#include <assert.h>
#include <pthread.h>
#include <unistd.h>

_Atomic int worker_ready = 0;
_Atomic int main_ready = 0;

static void *worker1(void *arg) {
	(void)arg;
	assert(!pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
	assert(!pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL));

	worker_ready = 1;

	while (1) sleep(1);

	return NULL;
}

static void *worker2(void *arg) {
	(void) arg;
	assert(!pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));

	worker_ready = 1;

	while(!main_ready);

	// Cancellation point - we should cancel right now
	sleep(1);

	assert(!"Expected to be cancelled!");
	__builtin_unreachable();
}

static void *worker3(void *arg) {
	(void) arg;
	assert(!pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));

	worker_ready = 1;

	while(!main_ready);

	// Cancellation point - we should cancel right now
	pthread_testcancel();

	assert(!"Expected to be cancelled!");
	__builtin_unreachable();
}

static void check_result(pthread_t thread) {

	void *ret_val = NULL;
	int ret = pthread_join(thread, &ret_val);
	assert(!ret);
	assert(ret_val == PTHREAD_CANCELED);
}

int main() {
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, &worker1, NULL);
	assert(!ret);

	while (!worker_ready);
	ret = pthread_cancel(thread);
	assert(!ret);
	check_result(thread);

	worker_ready = 0;
	ret = pthread_create(&thread, NULL, &worker2, NULL);
	assert(!ret);

	while(!worker_ready);
	ret = pthread_cancel(thread);
	assert(!ret);
	main_ready = 1;
	check_result(thread);

	worker_ready = 0;
	ret = pthread_create(&thread, NULL, &worker3, NULL);
	assert(!ret);

	while(!worker_ready);
	ret = pthread_cancel(thread);
	assert(!ret);
	main_ready = 1;
	check_result(thread);

	return 0;
}
