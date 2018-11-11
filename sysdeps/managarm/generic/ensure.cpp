
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

#include <hel.h>
#include <hel-syscalls.h>

void __frigg_assert_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	mlibc::panicLogger() << "In function " << function
			<< ", file " << file << ":" << line << "\n"
			<< "__ensure(" << assertion << ") failed" << frg::endlog;
}

namespace mlibc {
	void sys_libc_log(const char *message) {
		size_t n = 0;
		while(message[n])
			n++;
		HEL_CHECK(helLog(message, n));
		HEL_CHECK(helLog("\n", 1));
	}

	void sys_libc_panic() {
		const char *message = "mlibc: Panic!";
		size_t n = 0;
		while(message[n])
			n++;
		helPanic(message, n);
	}
}

