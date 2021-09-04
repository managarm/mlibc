#include <pthread.h>
#include <assert.h>
#include <errno.h>

static void test_detachstate() {
	pthread_attr_t attr;
	assert(!pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
	int detachstate;
	assert(!pthread_attr_getdetachstate(&attr, &detachstate));
	assert(detachstate == PTHREAD_CREATE_DETACHED);
	assert(pthread_attr_setdetachstate(&attr, 2* (PTHREAD_CREATE_DETACHED +
				PTHREAD_CREATE_JOINABLE)) == EINVAL);
}

int main() {
	test_detachstate();
}
