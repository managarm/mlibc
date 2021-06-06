#include <assert.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

int main() {
	const char *shortopts = "b::cdef";

	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	int test_argc = 7;

	char *test_argv[] = {
		"dummy",
		"--foo",
		"abc",
		"-b",
		"abc",
		"abc",
		"abc"
	};

	int c;
	c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	assert(c == 'f');

	c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	assert(!strcmp(optarg, "abc"));
	assert(c == 'b');

	c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	assert(c == -1);

	// we have 2 non-option arguments
	assert((optind + 2) == test_argc);
}
