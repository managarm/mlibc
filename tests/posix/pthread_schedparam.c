#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

int main() {
	struct sched_param param = {
		.sched_priority = 100,
	};

	int policy = 0xDEADBEEF;

	int ret = pthread_getschedparam(pthread_self(), &policy, &param);
	assert(policy == SCHED_OTHER);
	assert(!ret);

	param.sched_priority = 10;

	ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
	assert(!ret || ret == EPERM);

	if(ret == EPERM) {
		exit(0);
	}

	param.sched_priority = 0xDEADBEEF;

	ret = pthread_getschedparam(pthread_self(), &policy, &param);
	assert(policy == SCHED_FIFO);
	assert(param.sched_priority == 10);
	assert(!ret);

	return 0;
}
