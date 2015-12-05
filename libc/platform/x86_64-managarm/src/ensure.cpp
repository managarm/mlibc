
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#pragma GCC visibility push(hidden)

#include <mlibc/ensure.h>

#include <frigg/debug.hpp>

#include <hel.h>
#include <hel-syscalls.h>

#pragma GCC visibility pop

void __ensure_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	frigg::panicLogger.log() << "In function " << function
			<< ", file " << file << ":" << line << "\n"
			<< "__ensure(" << assertion << ") failed" << frigg::EndLog();
}

