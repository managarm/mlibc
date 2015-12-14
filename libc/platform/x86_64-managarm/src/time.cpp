
// for time()
#include <time.h>
// for gettimeofday()
#include <sys/time.h>

#include <mlibc/ensure.h>

#pragma GCC visibility push(hidden)

#include <frigg/debug.hpp>

#pragma GCC visibility pop

time_t time(time_t *out){
	frigg::infoLogger.log() << "mlibc: Broken time() called!" << frigg::EndLog();
	time_t result = 0;
	if(out)
		*out = result;
	return result;
}

int gettimeofday(struct timeval *__restrict result, void *__restrict unused) {
	frigg::infoLogger.log() << "mlibc: Broken gettimeofday() called!" << frigg::EndLog();
	__ensure(!unused);
	result->tv_sec = 0;
	result->tv_usec = 0;
	return 0;
}

