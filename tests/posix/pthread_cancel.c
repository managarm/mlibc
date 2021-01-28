#include <assert.h>
#include <pthread.h>
#include <unistd.h>

_Atomic int ready = 0;

static void *worker(void *arg) {
	(void)arg;
	assert(!pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL));
	assert(!pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL));

	ready = 1;

	while (1) sleep(1);

	return NULL;
}

int main() {
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, &worker, NULL);
	assert(!ret);

	while (!ready);

	ret = pthread_cancel(thread);
	assert(!ret);

	void *ret_val = NULL;
	ret = pthread_join(thread, &ret_val);
	assert(!ret);
	assert(ret_val == PTHREAD_CANCELED);
	return 0;
}
