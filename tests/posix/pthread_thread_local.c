#include <stdio.h>
#include <pthread.h>
#include <assert.h>
 
_Thread_local unsigned int rage = 9999; 
_Thread_local unsigned int uninitialized;
 
void *check_rage(void *arg)
{
	(void)arg;
    fprintf(stderr, "Rage counter for thread a: %d\n", rage, &rage);
	fflush(stderr);
	assert(rage == 9999);
	assert(uninitialized == 0);

	++rage;
	++uninitialized;
    fprintf(stderr, "Rage counter for thread a: %d\n", rage);
	fflush(stderr);
	assert(rage == 10000);
	assert(uninitialized == 1);
	return NULL;
}
 
int main()
{
    fprintf(stderr, "Rage counter for main: %d\n", rage);
	fflush(stderr);
	assert(rage == 9999);
	assert(uninitialized == 0);

	++rage;
	++uninitialized;
    fprintf(stderr, "Rage counter for main: %d\n", rage);
	fflush(stderr);
	assert(rage == 10000);
	assert(uninitialized == 1);

	pthread_t thd;
	pthread_create(&thd, NULL, check_rage, NULL);
	pthread_join(thd, NULL);

    fprintf(stderr, "Rage counter for main: %d\n", rage);
	fflush(stderr);
	assert(rage == 10000);
	assert(uninitialized == 1);
}
