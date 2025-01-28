
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <bits/ensure.h>

[[gnu::noreturn]] void __assert_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	fprintf(stderr, "In function %s, file %s:%d: Assertion '%s' failed!\n",
			function, file, line, assertion);
	abort();
}
