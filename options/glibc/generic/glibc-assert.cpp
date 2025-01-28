#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bits/ensure.h>

[[gnu::noreturn]] void __assert_fail_perror(int errno, const char *file, unsigned int line,
		const char *function) {
	char *errormsg = strerror(errno);
	fprintf(stderr, "In function %s, file %s:%d: Errno '%s' failed!\n",
			function, file, line, errormsg);
	abort();
}
