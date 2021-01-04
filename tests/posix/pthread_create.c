#include <assert.h>
#include <time.h>
#include <pthread.h>

int variable = 0;

static void *worker(void *arg) {
	(void) arg;
	variable = 1;
	return NULL;
}

int main() {
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, &worker, NULL);
	assert(!ret);

	ret = pthread_join(thread, NULL);
	assert(!ret);
	assert(variable == 1);
	return 0;
}
