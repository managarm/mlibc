#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int main() {
	int ret = pthread_setname_np(pthread_self(), "mlibc-test-123");
	assert(!ret);

	char buf[16];
	ret = pthread_getname_np(pthread_self(), buf, 16);
	assert(!ret);
	assert(!strcmp("mlibc-test-123", buf));

	ret = pthread_setname_np(pthread_self(), "mlibc-test-123-too-long");
	assert(ret == ERANGE);

	ret = pthread_getname_np(pthread_self(), buf, 1);
	assert(ret == ERANGE);

	ret = pthread_getname_np(pthread_self(), buf, 15);
	assert(ret == ERANGE);

	ret = pthread_getname_np(pthread_self(), buf, 16);
	assert(!ret);
	assert(!strcmp("mlibc-test-123", buf));

	return 0;
}
