#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#if defined(__linux__)

#include <pthread.h>

size_t COUNTER_EXPIRATIONS = 3;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
size_t count = 0;

void timer_handler(union sigval val) {
	(void) val;
	pthread_mutex_lock(&mutex);
	count++;
	fprintf(stderr, "new count %zu\n", count);
	if(count >= COUNTER_EXPIRATIONS) {
		pthread_cond_signal(&cond);
	}
	pthread_mutex_unlock(&mutex);
}

static void test_thread_notify() {
	timer_t timer;
	struct sigevent sev = {
		.sigev_value = {
			.sival_ptr = &timer,
		},
		.sigev_notify = SIGEV_THREAD,
	};
	sev.sigev_notify_function = timer_handler;
	sev.sigev_notify_attributes = NULL;

	int ret = timer_create(CLOCK_MONOTONIC, &sev, &timer);
	assert(ret == 0);

	struct itimerspec its = {
		.it_interval = {
			.tv_sec = 1,
			.tv_nsec = 0,
		},
		.it_value = {
			.tv_sec = 1,
			.tv_nsec = 0,
		},
	};
	ret = timer_settime(timer, 0, &its, NULL);
	assert(ret == 0);

	pthread_mutex_lock(&mutex);

	while(count < COUNTER_EXPIRATIONS)
		pthread_cond_wait(&cond, &mutex);

	pthread_mutex_unlock(&mutex);

	ret = timer_delete(timer);
	assert(ret == 0);
}

#endif

int main() {
	struct sigevent evp;
	memset(&evp, 0, sizeof(evp));

	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &set, 0);
	evp.sigev_notify = SIGEV_SIGNAL;
	evp.sigev_signo = SIGUSR1;

	struct timeval start;
	gettimeofday(&start, NULL);

	timer_t timer;
	if (timer_create(CLOCK_MONOTONIC, &evp, &timer)) {
		perror("timer_create");
		exit(1);
	}

	struct itimerspec spec;
	memset(&spec, 0, sizeof(spec));
	spec.it_value.tv_sec = 1;
	spec.it_value.tv_nsec = 0;

	int sig;
	timer_settime(timer, 0, &spec, NULL);

	struct itimerspec its = {};
	int ret = timer_gettime(timer, &its);
	assert(ret == 0);
	assert(its.it_value.tv_sec || its.it_value.tv_nsec);

	sigwait(&set, &sig);

	struct timeval end;
	gettimeofday(&end, NULL);

	double diff = end.tv_sec - start.tv_sec;
	diff += (end.tv_usec - start.tv_usec) / 1000000.0;
	assert(diff >= 1.0);

#if defined(__linux__)
	test_thread_notify();
#endif

	timer_delete(timer);

	return 0;
}
