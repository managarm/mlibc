#include <pthread.h>
#include <assert.h>
#include <errno.h>
#include <alloca.h>
#include <string.h>
#include <sys/mman.h>

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

static void test_guardsize() {
	pthread_attr_t attr;
	assert(!pthread_attr_init(&attr));
	assert(!pthread_attr_setguardsize(&attr, 0));
	size_t guardsize;
	assert(!pthread_attr_getguardsize(&attr, &guardsize));
	assert(!guardsize);
}

static void test_scope() {
	pthread_attr_t attr;
	assert(!pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM));
	int scope;
	assert(!pthread_attr_getscope(&attr, &scope));
	assert(scope == PTHREAD_SCOPE_SYSTEM);
	assert(pthread_attr_setscope(&attr, 2* (PTHREAD_SCOPE_SYSTEM +
				PTHREAD_SCOPE_PROCESS)) == EINVAL);
}

static void test_inheritsched() {
	pthread_attr_t attr;
	assert(!pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED));
	int inheritsched;
	assert(!pthread_attr_getinheritsched(&attr, &inheritsched));
	assert(inheritsched == PTHREAD_INHERIT_SCHED);
	assert(pthread_attr_setinheritsched(&attr, 2* (PTHREAD_INHERIT_SCHED +
				PTHREAD_EXPLICIT_SCHED)) == EINVAL);
}

static void test_schedparam() {
	pthread_attr_t attr;
	struct sched_param init_param = {0};
	assert(!pthread_attr_setschedparam(&attr, &init_param));
	struct sched_param param = {1};
	assert(!pthread_attr_getschedparam(&attr, &param));
	assert(param.sched_priority == init_param.sched_priority);
}

static void test_schedpolicy() {
	pthread_attr_t attr;
	assert(!pthread_attr_setschedpolicy(&attr, SCHED_FIFO));
	int policy;
	assert(!pthread_attr_getschedpolicy(&attr, &policy));
	assert(policy == SCHED_FIFO);
	assert(pthread_attr_setinheritsched(&attr, 2* (SCHED_FIFO + SCHED_RR +
				SCHED_OTHER)) == EINVAL);
}

static void *stackaddr_worker(void *arg) {
	void *addr = *(void**)arg;

	void *sp;
#if defined(__x86_64__)
	asm volatile ("mov %%rsp, %0" : "=r"(sp));
#elif defined(__aarch64__)
	asm volatile ("mov %0, sp" : "=r"(sp));
#elif defined (__riscv)
	asm volatile ("mv %0, sp" : "=r"(sp));
#else
#	error Unknown architecture
#endif

	// Check if our stack pointer is in a sane range.
	assert(addr >= sp);
	return NULL;
}

static void test_stackaddr() {
	pthread_attr_t attr;
	assert(!pthread_attr_init(&attr));
	size_t size;
	assert(!pthread_attr_getstacksize(&attr, &size));
	void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0) + size;
	assert(!pthread_attr_setstackaddr(&attr, addr));
	assert(!pthread_attr_setguardsize(&attr, 0));
	void *new_addr;
	assert(!pthread_attr_getstackaddr(&attr, &new_addr));
	assert(new_addr == addr);

	pthread_t thread;
	assert(!pthread_create(&thread, &attr, stackaddr_worker, &addr));
	assert(!pthread_join(thread, NULL));
}

static void test_stack() {
	pthread_attr_t attr;
	void *stackaddr = (void*)1;
	size_t stacksize = PTHREAD_STACK_MIN;

	assert(!pthread_attr_setstack(&attr, stackaddr, stacksize));
	void *new_addr;
	size_t new_size;
	assert(!pthread_attr_getstack(&attr, &new_addr, &new_size));
	assert(new_addr == stackaddr);
	assert(new_size == stacksize);
}

static void test_affinity() {
	pthread_attr_t attr;
	cpu_set_t set = {0};
	assert(!pthread_attr_init(&attr));
	assert(!pthread_attr_setaffinity_np(&attr, 1, &set));

	cpu_set_t other_set = {0};
	assert(!pthread_attr_getaffinity_np(&attr, 1, &set));

	assert(!memcmp(&set, &other_set, sizeof(cpu_set_t)));
}

static void test_sigmask() {
	pthread_attr_t attr;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigaddset(&set, SIGCANCEL);

	assert(!pthread_attr_init(&attr));
	assert(!pthread_attr_setsigmask_np(&attr, &set));

	sigset_t other_set;
	sigemptyset(&other_set);

	assert(!pthread_attr_getsigmask_np(&attr, &other_set));

	assert(sigismember(&other_set, SIGUSR1));
	// Test whether internal signals get filtered properly.
	assert(!sigismember(&other_set, SIGCANCEL));
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
	assert(!*(char*)(stack + other_stacksize - 1));
}

int main() {
	test_detachstate();
	test_stacksize();
	test_guardsize();
	test_scope();
	test_inheritsched();
	test_schedparam();
	test_schedpolicy();
	test_stackaddr();
	test_stack();
	test_affinity();
	test_sigmask();
	test_getattrnp();
}
