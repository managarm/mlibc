
#include <assert.h>

#include <mlibc/ensure.h>

void __assert_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	__ensure(!"Not implemented");
}

