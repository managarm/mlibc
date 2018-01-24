
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <bits/ensure.h>

#include <frigg/debug.hpp>

#include <hel.h>
#include <hel-syscalls.h>

void __assert_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	fprintf(stderr, "In function %s, file %s:%d: Assertion '%s' failed!\n",
			function, file, line, assertion);
	abort();
}

void __ensure_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	frigg::panicLogger() << "In function " << function
			<< ", file " << file << ":" << line << "\n"
			<< "__ensure(" << assertion << ") failed" << frigg::endLog;
}

