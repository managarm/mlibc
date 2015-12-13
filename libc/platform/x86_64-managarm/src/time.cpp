
#include <time.h>

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

