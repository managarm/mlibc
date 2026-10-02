#include <assert.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stddef.h>

#ifdef USE_HOST_LIBC
#define LIBFOO "libnative-foo.so"
#else
#define LIBFOO "libfoo.so"
#endif

#define NUM_THREADS 4

static int *(*get_tls_fn)(void);

static void *thread_worker(void *arg) {
	long id = (long)arg;
	int *ptr = get_tls_fn();

	assert(*ptr == 42);
	*ptr += (int)id;
	assert(*ptr == 42 + (int)id);

	return (void *)ptr;
}

int main(void) {
	void *handle = dlopen(LIBFOO, RTLD_NOW | RTLD_LOCAL);
	assert(handle != NULL);

	*(void **)(&get_tls_fn) = dlsym(handle, "get_tls");
	assert(get_tls_fn != NULL);

	pthread_t th[NUM_THREADS];
	void *addrs[NUM_THREADS];

	for (long i = 0; i < NUM_THREADS; i++) {
		int ret = pthread_create(&th[i], NULL, thread_worker, (void *)(i + 1));
		assert(ret == 0);
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		int ret = pthread_join(th[i], &addrs[i]);
		assert(ret == 0);
	}

	// verify TLS isolation: each thread must have a distinct storage address
	for (int i = 0; i < NUM_THREADS; i++) {
		for (int j = i + 1; j < NUM_THREADS; j++) {
			assert(addrs[i] != addrs[j]);
		}
	}

	// main thread's initial value must remain intact
	int *main_ptr = get_tls_fn();
	assert(*main_ptr == 42);

	dlclose(handle);
	return 0;
}
