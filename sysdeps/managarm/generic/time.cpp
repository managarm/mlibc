
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

