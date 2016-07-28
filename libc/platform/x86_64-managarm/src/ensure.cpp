
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <mlibc/ensure.h>

#pragma GCC visibility push(hidden)

#include <frigg/debug.hpp>

#include <hel.h>
#include <hel-syscalls.h>

#pragma GCC visibility pop

void __assert_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	frigg::panicLogger() << "In function " << function
			<< ", file " << file << ":" << line << "\n"
			<< "assert(" << assertion << ") failed" << frigg::endLog;
}

void __ensure_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	frigg::panicLogger() << "In function " << function
			<< ", file " << file << ":" << line << "\n"
			<< "__ensure(" << assertion << ") failed" << frigg::endLog;
}

