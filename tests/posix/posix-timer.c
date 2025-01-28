#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

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
	sigwait(&set, &sig);

	struct timeval end;
	gettimeofday(&end, NULL);
	timer_delete(timer);

	double diff = end.tv_sec - start.tv_sec;
	diff += (end.tv_usec - start.tv_usec) / 1000000.0;
	assert(diff >= 1.0);

	return 0;
}
