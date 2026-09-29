#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <threads.h>

static _Thread_local int thread_idx = -1;

static int *signal_counts;

static void sig_handler(int sig) {
	assert(thread_idx != -1);
	int expected_sig = thread_idx + SIGRTMIN;
	assert(sig == expected_sig);
	__atomic_fetch_add(&signal_counts[thread_idx], 1, __ATOMIC_RELAXED);
}

static int worker(void *arg) {
	int idx = *(int *)arg;
	thread_idx = idx;

	int sig = idx + SIGRTMIN;
	int ret = raise(sig);
	assert(ret == 0);

	return 0;
}

int main(void) {
	// Test that out of range signal numbers return EINVAL in raise()
	errno = 0;
	int res = raise(-1);
	assert(res != 0);
	assert(errno == EINVAL);

	errno = 0;
	res = raise(1000000);
	assert(res != 0);
	assert(errno == EINVAL);

	int num_threads = SIGRTMAX - SIGRTMIN;
	assert(num_threads > 0);

	signal_counts = calloc(num_threads, sizeof(int));
	assert(signal_counts != NULL);

	for (int i = 0; i < num_threads; i++) {
		int sig = SIGRTMIN + i;
		void (*ret)(int) = signal(sig, sig_handler);
		assert(ret != SIG_ERR);
	}

	thrd_t *threads = malloc(num_threads * sizeof(thrd_t));
	int *indices = malloc(num_threads * sizeof(int));
	assert(threads != NULL);
	assert(indices != NULL);

	for (int i = 0; i < num_threads; i++) {
		indices[i] = i;
		int ret = thrd_create(&threads[i], worker, &indices[i]);
		assert(ret == thrd_success);
	}

	for (int i = 0; i < num_threads; i++) {
		int thread_res = -1;
		int ret = thrd_join(threads[i], &thread_res);
		assert(ret == thrd_success);
		assert(thread_res == 0);
	}

	for (int i = 0; i < num_threads; i++) {
		int count = __atomic_load_n(&signal_counts[i], __ATOMIC_RELAXED);
		assert(count == 1);
	}

	free(threads);
	free(indices);
	free(signal_counts);
	return 0;
}
