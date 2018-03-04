
#include <bits/ensure.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include <hel.h>
#include <hel-syscalls.h>
#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

namespace mlibc {

int sys_clock_get(int clock, time_t *secs, long *nanos) {
	if(clock == CLOCK_MONOTONIC) {
		uint64_t tick;
		HEL_CHECK(helGetClock(&tick));
		*secs = tick / 1000000000;
		*nanos = tick % 1000000000;
	}else if(clock == CLOCK_REALTIME) {
		frigg::infoLogger() << "\e[31mmlibc: clock_gettime does not support CLOCK_REALTIME"
				"\e[39m" << frigg::endLog;
		uint64_t tick;
		HEL_CHECK(helGetClock(&tick));
		*secs = tick / 1000000000;
		*nanos = tick % 1000000000;
	}else if(clock == CLOCK_PROCESS_CPUTIME_ID) {
		frigg::infoLogger() << "\e[31mmlibc: clock_gettime does not support the CPU time clocks"
				"\e[39m" << frigg::endLog;
		*secs = 0;
		*nanos = 0;
	}else{
		frigg::panicLogger() << "mlibc: Unexpected clock " << clock << frigg::endLog;
	}
	return 0;
}

} //namespace mlibc

