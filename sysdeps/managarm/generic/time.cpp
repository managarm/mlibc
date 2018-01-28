
// for time()
#include <time.h>
// for gettimeofday()
#include <sys/time.h>

#include <bits/ensure.h>

#include <frigg/debug.hpp>

#include <mlibc/sysdeps.hpp>

namespace mlibc {

int sys_clock_get(time_t *secs) {
	frigg::infoLogger() << "mlibc: Broken time() called!" << frigg::endLog;
	*secs = 0;
	return 0;
}

} //namespace mlibc

int gettimeofday(struct timeval *__restrict result, void *__restrict unused) {
	frigg::infoLogger() << "mlibc: Broken gettimeofday() called!" << frigg::endLog;
	__ensure(!unused);
	result->tv_sec = 0;
	result->tv_usec = 0;
	return 0;
}

