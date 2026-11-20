#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static long elapsed_ns(const struct timespec *start, const struct timespec *end) {
	return (end->tv_sec - start->tv_sec) * 1000000000L + (end->tv_nsec - start->tv_nsec);
}

int main(void) {
	struct timespec req = {0, 100000000};
	struct timespec start, end;

	// Relative sleep.
	assert(clock_gettime(CLOCK_MONOTONIC, &start) == 0);
	assert(clock_nanosleep(CLOCK_MONOTONIC, 0, &req, NULL) == 0);
	assert(clock_gettime(CLOCK_MONOTONIC, &end) == 0);
	assert(elapsed_ns(&start, &end) >= 100000000L);

	// Absolute sleep until a deadline in the future.
	assert(clock_gettime(CLOCK_MONOTONIC, &start) == 0);
	struct timespec deadline = start;
	deadline.tv_nsec += 100000000;
	if (deadline.tv_nsec >= 1000000000) {
		deadline.tv_sec += 1;
		deadline.tv_nsec -= 1000000000;
	}
	assert(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL) == 0);
	assert(clock_gettime(CLOCK_MONOTONIC, &end) == 0);
	assert(elapsed_ns(&start, &end) >= 100000000L);

	// Absolute sleep until a deadline in the past returns immediately.
	struct timespec past = {0, 0};
	assert(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &past, NULL) == 0);

	// Invalid timespecs are rejected for both relative and absolute sleeps.
	struct timespec bad_nsec = {0, 1000000000};
	struct timespec neg_nsec = {0, -1};
	struct timespec neg_sec = {-1, 0};
	assert(clock_nanosleep(CLOCK_MONOTONIC, 0, &bad_nsec, NULL) == EINVAL);
	assert(clock_nanosleep(CLOCK_MONOTONIC, 0, &neg_nsec, NULL) == EINVAL);
	assert(clock_nanosleep(CLOCK_MONOTONIC, 0, &neg_sec, NULL) == EINVAL);
	assert(clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &bad_nsec, NULL) == EINVAL);

	// The calling thread's CPU-time clock and unknown clocks are rejected.
	assert(clock_nanosleep(CLOCK_THREAD_CPUTIME_ID, 0, &req, NULL) == EINVAL);
	assert(clock_nanosleep((clockid_t)12345, 0, &req, NULL) == EINVAL);

	return 0;
}
