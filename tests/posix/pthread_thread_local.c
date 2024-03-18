#include <stdio.h>
#include <pthread.h>
#include <assert.h>
 
_Thread_local unsigned int counter = 9999; 
_Thread_local unsigned int uninitialized;
 
void *check_counter(void *arg)
{
	(void)arg;
	fprintf(stderr, "counter for worker thread: %d, at %p\n", counter, &counter);
	fflush(stderr);
	assert(counter == 9999);

	fprintf(stderr, "uninitialized data for worker thread: %d, at %p\n", uninitialized, &uninitialized);
	fflush(stderr);
	assert(uninitialized == 0);

	++counter;
	++uninitialized;
	fprintf(stderr, "counter for worker thread: %d\n", counter);
	fflush(stderr);
	assert(counter == 10000);
	assert(uninitialized == 1);
	return NULL;
}
 
int main()
{
	fprintf(stderr, "counter for main thread: %d, at %p\n", counter, &counter);
	fflush(stderr);
	assert(counter == 9999);

	fprintf(stderr, "uninitialized data for main thread: %d, at %p\n", uninitialized, &uninitialized);
	fflush(stderr);
	assert(uninitialized == 0);

	++counter;
	++uninitialized;
	fprintf(stderr, "counter for main: %d\n", counter);
	fflush(stderr);
	assert(counter == 10000);
	assert(uninitialized == 1);

	pthread_t thd;
	pthread_create(&thd, NULL, check_counter, NULL);
	pthread_join(thd, NULL);

	fprintf(stderr, "counter for main: %d\n", counter);
	fflush(stderr);
	assert(counter == 10000);
	assert(uninitialized == 1);

	return 0;
}
