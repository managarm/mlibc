#include <signal.h>
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile int alarms_fired = 0;

static void sigalrm_handler(int signal) {
	if(signal == SIGALRM)
		alarms_fired++;
}

int main() {
	signal(SIGALRM, sigalrm_handler);

	alarms_fired = 0;

	unsigned int ret = alarm(10);
	assert(!ret);

	sleep(1);

	ret = alarm(1);
	assert(ret == 9);

	sleep(2);

	if(alarms_fired != 1) {
		fprintf(stderr, "alarm handler fired %u times instead of 1\n", alarms_fired);
		exit(1);
	}

	return 0;
}
