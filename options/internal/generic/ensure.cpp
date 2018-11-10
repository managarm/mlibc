
#include <bits/ensure.h>
#include <mlibc/debug.hpp>

void __ensure_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	mlibc::panicLogger() << "In function " << function
			<< ", file " << file << ":" << line << "\n"
			<< "__ensure(" << assertion << ") failed" << frg::endlog;
}

