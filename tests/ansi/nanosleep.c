#include <assert.h>
#include <stddef.h>
#include <time.h>

int main(void) {
	struct timespec req = {0, 500000000};
	struct timespec rem = {0, 0};
	struct timespec start = {0, 0};
	struct timespec end = {0, 0};

	int ret = clock_gettime(CLOCK_MONOTONIC, &start);
	assert(ret == 0);

	ret = nanosleep(&req, &rem);
	assert(ret == 0);

	ret = clock_gettime(CLOCK_MONOTONIC, &end);
	assert(ret == 0);

	long elapsed = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
	assert(elapsed >= 500000000L);

	// check whether passing a nullptr as the remainder arg works
	ret = clock_gettime(CLOCK_MONOTONIC, &start);
	assert(ret == 0);

	ret = nanosleep(&req, NULL);
	assert(ret == 0);

	ret = clock_gettime(CLOCK_MONOTONIC, &end);
	assert(ret == 0);

	elapsed = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);
	assert(elapsed >= 500000000L);

	return 0;
}
