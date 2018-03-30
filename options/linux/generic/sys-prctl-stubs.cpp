
#include <bits/ensure.h>
#include <sys/prctl.h>

#include <frigg/debug.hpp>

int prctl(int, ...) {
	frigg::infoLogger() << "\e[31mmlibc: prctl() is not implemented correctly\e[39m"
			<< frigg::endLog;
	return 0;
}

