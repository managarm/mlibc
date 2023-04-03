#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

static void test_affinity() {
	pthread_attr_t attr;
	cpu_set_t set = {0};
	assert(!pthread_attr_init(&attr));
	assert(!pthread_attr_setaffinity_np(&attr, 1, &set));

	cpu_set_t other_set = {0};
	assert(!pthread_attr_getaffinity_np(&attr, 1, &set));

	assert(!memcmp(&set, &other_set, sizeof(cpu_set_t)));

	pthread_attr_destroy(&attr);
}

#if !defined(USE_HOST_LIBC) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 32)
static void test_sigmask() {
	pthread_attr_t attr;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
#ifndef USE_HOST_LIBC
	sigaddset(&set, SIGCANCEL);
#endif

	assert(!pthread_attr_init(&attr));
	assert(!pthread_attr_setsigmask_np(&attr, &set));

	sigset_t other_set;
	sigemptyset(&other_set);

	assert(!pthread_attr_getsigmask_np(&attr, &other_set));

	assert(sigismember(&other_set, SIGUSR1));
#ifndef USE_HOST_LIBC
	// Test whether internal signals get filtered properly.
	assert(!sigismember(&other_set, SIGCANCEL));
#endif

	pthread_attr_destroy(&attr);
}

static void *getattr_worker(void *arg) {
	(void)arg;
	return NULL;
}

static void test_getattrnp() {
	pthread_attr_t attr;
	size_t stacksize = PTHREAD_STACK_MIN;
	assert(!pthread_attr_init(&attr));
	assert(!pthread_attr_setstacksize(&attr, stacksize));

	pthread_t thread;
	assert(!pthread_create(&thread, &attr, getattr_worker, NULL));
	assert(!pthread_getattr_np(thread, &attr));
	size_t other_stacksize;
	assert(!pthread_attr_getstacksize(&attr, &other_stacksize));
	assert(other_stacksize == stacksize);
	assert(!pthread_join(thread, NULL));

	pthread_t own_thread = pthread_self();
	void *stack;
	assert(!pthread_getattr_np(own_thread, &attr));
	assert(!pthread_attr_getstack(&attr, &stack, &other_stacksize));
	assert(stack);
	assert(other_stacksize);
	// Check that we can read from the highest byte returned.
	// pthread_getattr_np() should return the lowest byte
	// of the stack.
	printf("highest byte: %hhu\n", *(char *)(stack + other_stacksize - 1));

	pthread_attr_destroy(&attr);
}
#endif // !defined(USE_HOST_LIBC) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 32)

int main() {
	test_affinity();
#if !defined(USE_HOST_LIBC) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 32)
	test_sigmask();
	test_getattrnp();
#endif

	return 0;
}
