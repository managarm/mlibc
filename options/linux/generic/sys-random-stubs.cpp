
#include <sys/random.h>
#include <bits/ensure.h>

#include <frigg/debug.hpp>

ssize_t getrandom(void *buffer, size_t max_size, unsigned int flags) {
	frigg::infoLogger() << "\e[31mmlibc: getrandom() is a no-op\e[39m" << frigg::endLog;
	return max_size;
}

