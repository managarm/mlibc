#include <stdint.h>
#include <assert.h>
#include <pthread.h>

_Atomic uintptr_t cleanup_val = 0;

static void cleanup(void *arg) {
	cleanup_val = (uintptr_t)arg;
}

static void *worker(void *arg) {
	(void)arg;

	pthread_cleanup_push(cleanup, (void *)1);
	pthread_cleanup_push(cleanup, (void *)2);
	pthread_cleanup_push(cleanup, (void *)3);
	pthread_cleanup_push(cleanup, (void *)4);

	pthread_cleanup_pop(1);
	assert(cleanup_val == 4);

	pthread_cleanup_pop(0);
	assert(cleanup_val == 4);

	pthread_exit(NULL);

	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);

	return NULL;
}

int main() {
	pthread_t thread;
	assert(!pthread_create(&thread, NULL, &worker, NULL));
	assert(!pthread_join(thread, NULL));

	assert(cleanup_val == 1);

	return 0;
}

