
#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

#include <hel-syscalls.h>
#include <hel.h>

void __frigg_assert_fail(
    const char *assertion, const char *file, unsigned int line, const char *function
) {
	mlibc::panicLogger() << "In function " << function << ", file " << file << ":" << line << "\n"
	                     << "__ensure(" << assertion << ") failed" << frg::endlog;
}

namespace mlibc {
void sys_libc_log(const char *message) {
	// This implementation is inherently signal-safe.
	size_t n = 0;
	while (message[n])
		n++;
	HEL_CHECK(helLog(kHelLogSeverityInfo, message, n));
}

void sys_libc_panic() {
	// This implementation is inherently signal-safe.
	const char *message = "mlibc: Panic!";
	size_t n = 0;
	while (message[n])
		n++;
	helPanic(message, n);
}
} // namespace mlibc
