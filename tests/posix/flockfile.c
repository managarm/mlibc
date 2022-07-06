#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static void *worker(void *arg) {
	(void)arg;
	flockfile(stdout);
	fputs_unlocked("hello from worker", stdout);
	funlockfile(stdout);
	return NULL;
}

int main() {
	// Check that recursive locking works.
	assert(!ftrylockfile(stdout));
	flockfile(stdout);
	flockfile(stdout);
	funlockfile(stdout);
	funlockfile(stdout);
	funlockfile(stdout);

	assert(!ftrylockfile(stdout));

	pthread_t thread;
	int ret = pthread_create(&thread, NULL, &worker, NULL);
	assert(!ret);

	sleep(1);
	funlockfile(stdout);

	assert(!pthread_join(thread, NULL));
	return 0;
}
