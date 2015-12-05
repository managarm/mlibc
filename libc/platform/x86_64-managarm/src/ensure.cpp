
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <mlibc/ensure.h>

#include <hel.h>
#include <hel-syscalls.h>

void __ensure_fail(const char *assertion, const char *file, unsigned int line,
		const char *function) {
	helPanic("x", 1);
}

