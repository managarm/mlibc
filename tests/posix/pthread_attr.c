#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <alloca.h>

static void test_detachstate() {
	pthread_attr_t attr;
	assert(!pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
	int detachstate;
	assert(!pthread_attr_getdetachstate(&attr, &detachstate));
	assert(detachstate == PTHREAD_CREATE_DETACHED);
	assert(pthread_attr_setdetachstate(&attr, 2* (PTHREAD_CREATE_DETACHED +
				PTHREAD_CREATE_JOINABLE)) == EINVAL);
}

static void *stacksize_worker(void *arg) {
	size_t default_stacksize = (*(size_t*)arg);
	size_t alloc_size = default_stacksize + default_stacksize/2;
	void *area = alloca(alloc_size);
	// If the allocated stack was not enough this will crash.
	*(volatile int*)(area + alloc_size) = 1;
	return NULL;
}

static void test_stacksize() {
	pthread_attr_t attr;
	assert(!pthread_attr_init(&attr));
	size_t stacksize;
	assert(!pthread_attr_getstacksize(&attr, &stacksize));
	assert(!pthread_attr_setstacksize(&attr, stacksize * 2));
	pthread_t thread;
	assert(!pthread_create(&thread, &attr, stacksize_worker, &stacksize));
	assert(!pthread_join(thread, NULL));
}

int main() {
	test_detachstate();
	test_stacksize();
}
