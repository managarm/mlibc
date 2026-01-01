#include <assert.h>
#include <sched.h>
#include <unistd.h>

int main(void) {
	struct sched_param param;
	param.sched_priority = 0;

	assert(sched_setscheduler(0, SCHED_BATCH, &param) == 0);

	int policy = sched_getscheduler(0);
	assert(policy == SCHED_BATCH);

	assert(sched_getparam(0, &param) == 0);
	assert(param.sched_priority == 0);

	return 0;
}
