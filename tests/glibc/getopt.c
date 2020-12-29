#include <assert.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>

int main() {
	const char *shortopts = "f:";

	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	int test_argc = 2;

	char *test_argv[] = {
		"dummy",
		"--foo",
		"abc"
	};

	int c;
	c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	assert(c == 'f');
	c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	assert(c == -1);
}
