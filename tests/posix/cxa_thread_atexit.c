#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

extern int __cxa_thread_atexit_impl(void (*)(void *), void *, void *);
extern void *__dso_handle;

static int sequence[4];
static size_t sequence_index;
static pthread_key_t key;
static int main_thread_destructor_ran;

static void thread_local_destructor(void *argument) {
	int value = (int)(intptr_t)argument;
	sequence[sequence_index++] = value;

	if (value == 2)
		assert(!__cxa_thread_atexit_impl(thread_local_destructor,
				(void *)(intptr_t)3, &__dso_handle));
}

static void key_destructor(void *argument) {
	(void)argument;
	sequence[sequence_index++] = 4;
}

static void *worker(void *argument) {
	(void)argument;
	assert(!pthread_setspecific(key, &key));
	assert(!__cxa_thread_atexit_impl(thread_local_destructor,
			(void *)(intptr_t)1, &__dso_handle));
	assert(!__cxa_thread_atexit_impl(thread_local_destructor,
			(void *)(intptr_t)2, &__dso_handle));
	return NULL;
}

static void main_thread_destructor(void *argument) {
	(void)argument;
	main_thread_destructor_ran = 1;
}

static void process_exit_handler(void) {
	assert(main_thread_destructor_ran);
}

int main(void) {
	assert(!pthread_key_create(&key, key_destructor));

	pthread_t thread;
	assert(!pthread_create(&thread, NULL, worker, NULL));
	assert(!pthread_join(thread, NULL));

	assert(sequence_index == 4);
	assert(sequence[0] == 2);
	assert(sequence[1] == 3);
	assert(sequence[2] == 1);
	assert(sequence[3] == 4);
	assert(!pthread_key_delete(key));

	assert(!atexit(process_exit_handler));
	assert(!__cxa_thread_atexit_impl(main_thread_destructor, NULL, &__dso_handle));
	return 0;
}
