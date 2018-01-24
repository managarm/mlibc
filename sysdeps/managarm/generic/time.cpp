
// for time()
#include <time.h>
// for gettimeofday()
#include <sys/time.h>

#include <bits/ensure.h>

#include <frigg/debug.hpp>

time_t time(time_t *out){
	frigg::infoLogger() << "mlibc: Broken time() called!" << frigg::endLog;
	time_t result = 0;
	if(out)
		*out = result;
	return result;
}

int gettimeofday(struct timeval *__restrict result, void *__restrict unused) {
	frigg::infoLogger() << "mlibc: Broken gettimeofday() called!" << frigg::endLog;
	__ensure(!unused);
	result->tv_sec = 0;
	result->tv_usec = 0;
	return 0;
}

