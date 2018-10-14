
#include <sys/random.h>
#include <bits/ensure.h>

#include <mlibc/debug.hpp>

ssize_t getrandom(void *buffer, size_t max_size, unsigned int flags) {
	mlibc::infoLogger() << "\e[31mmlibc: getrandom() is a no-op\e[39m" << frg::endlog;
	return max_size;
}

