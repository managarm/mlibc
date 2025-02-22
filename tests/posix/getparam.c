#include <assert.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>

int main() {
	struct sched_param param = {
		.sched_priority = 100,
	};

	int ret = sched_getparam(getpid(), &param);
	assert(!ret);

	ret = sched_setparam(getpid(), &param);
	assert(ret == 0);

	param.sched_priority = 0xD00DFEED;

	ret = sched_setparam(getpid(), &param);
	assert(ret == -1 && errno == EINVAL);

	return 0;
}
