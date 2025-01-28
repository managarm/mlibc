#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <error.h>

bool ran = false;

void test_progname(void) {
	ran = true;
	fprintf(stderr, "progname test");
}

int main() {
	assert(error_print_progname == NULL);
	error_print_progname = &test_progname;
	error(0, EINVAL, "test error #1");
	assert(ran == true);

	return 0;
}
