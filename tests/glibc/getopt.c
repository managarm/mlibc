#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define dump(x) fprintf(stderr, "getopt c '%c' (%d), optind %d ('%s'), optarg '%s', optopt '%c' (%d)\n", \
		(x), (x), optind, (size_t)optind >= COUNT_OF(test_argv) ? "out of range" : test_argv[optind], optarg, optopt, optopt);

void test1() {
	const char *shortopts = "b:cdef";

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

	optind = 0;
	int c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	dump(c)
	assert(c == 'f');

	c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	dump(c)
	assert(optarg && !strcmp(optarg, "abc"));
	assert(c == 'b');

	c = getopt_long(test_argc, test_argv, shortopts, longopts, NULL);
	dump(c);
	assert(c == -1);
	assert(optarg == NULL);

	// we have 2 non-option arguments
	assert((optind + 2) == test_argc);
}

void test2() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-c",
	};

	fputs("Situation: we pass a non-existant short option in argv\n", stderr);

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:", longopts, NULL);
	// Exprected result: return '?', optopt is set to the offender
	assert(c == '?');
	assert(optopt == 'c');

	fputs("Situation: we pass a non-existant short option in argv, while passing a leading colon in optstring\n", stderr);
	optind = 0;

	c = getopt_long(COUNT_OF(test_argv), test_argv, ":ab:", longopts, NULL);
	// Exprected result: return '?', optopt is set to the offender
	assert(c == '?');
	assert(optopt == 'c');

	fputs("Situation: we omit the required arg to a short option\n", stderr);
	optind = 0;

	c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:c:", longopts, NULL);
	// Exprected result: return '?', optopt is set to the offender
	assert(c == '?');
	assert(optopt == 'c');

	fputs("Situation: we omit the required arg to a short option, while passing a leading colon in optstring\n", stderr);
	optind = 0;

	c = getopt_long(COUNT_OF(test_argv), test_argv, ":ab:c:", longopts, NULL);
	// Exprected result: return ':', optopt is set to the offender
	assert(c == ':');
	assert(optopt == 'c');
}

void test3() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-cmanagarm",
	};

	fputs("Situation: we pass a concatenated argument to a short option\n", stderr);

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:c:", longopts, NULL);
	dump(c);
	// Exprected result:
	assert(c == 'c');
	assert(!strcmp(optarg, "managarm"));
}

void test4() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-acmanagarm",
	};

	fputs("Situation: we pass concatenated short options to getopt\n", stderr);

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:c:", longopts, NULL);
	assert(c == 'a');

	c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:c:", longopts, NULL);
	// Exprected result:
	assert(c == 'c');
	assert(!strcmp(optarg, "managarm"));
}

void test5() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, no_argument, NULL, 0}
	};

	char *test_argv[] = {
		"su",
		"-",
		"managarm",
		0
	};

	int test_argc = 3;

	fputs("Situation: testing `su - managarm`\n", stderr);

	optind = 0;
	int c = getopt_long(test_argc, test_argv, "ab:", longopts, NULL);
	dump(c);
	assert(c == -1);

	if (optind < test_argc && !strcmp(test_argv[optind], "-")) {
		++optind;
	}

	assert(optind < test_argc);
	assert(!strcmp(test_argv[optind++], "managarm"));
	assert(optind == test_argc);
}

void test6() {
	char *test_argv[] = {
		"telescope",
		"gemini://electrode.codes",
		"-S",
		0
	};

	int test_argc = 3;
	optind = 0;

	const struct option longopts[] = {
		{"colors",      no_argument,    NULL,   'C'},
		{"colours",	no_argument,    NULL,   'C'},
		{"help",	no_argument,	NULL,	'h'},
		{"safe",	no_argument,	NULL,	'S'},
		{"version",	no_argument,	NULL,	'v'},
		{NULL,		0,		NULL,	0},
	};

	int c = getopt_long(test_argc, test_argv, "Cc:hnST:v", longopts, NULL);
	dump(c);
	assert(c == 'S');
	assert(optind == 3);
	assert(!optarg);
}

void test7() {
	int c;

	static const struct option options[] = {
		{ "debug", no_argument, NULL, 'd' },
		{ "help", no_argument, NULL, 'h' },
		{ "version", no_argument, NULL, 'V' },
		{}
	};
	const char *command;

	char *test_argv[] = {
		"udevadm",
		"hwdb",
		"--update",
		0
	};

	int test_argc = 3;
	setenv("POSIXLY_CORRECT", "1", 1);
	optind = 0;

	while ((c = getopt_long(test_argc, test_argv, "+dhV", options, NULL)) >= 0) {
		switch (c) {
			case 'd':
				break;
			case 'h':
				break;
			case 'V':
				break;
			default:
				break;
		}
	}

	dump(c);
	command = test_argv[optind];
	assert(command);
	assert(!strcmp(command, "hwdb"));
}

void test8() {
	const char *shortopts = "b:cde";

	int foo = false;
	int bar = false;

	const struct option longopts[] = {
		{"foo", required_argument, &foo, 'x'},
		{"bar", required_argument, &bar, 'y'},
		{NULL, no_argument, NULL, 0}
	};

	int test_argc = 6;

	char *test_argv[] = {
		"dummy",
		"-foo",
		"abc",
		"-bar=def",
		"ghi",
		"jkl"
	};

	#ifdef __GLIBC__
		optind = 0;
	#else
		optreset = 1;
	#endif
	optopt = 0;
	int c = getopt_long_only(test_argc, test_argv, shortopts, longopts, NULL);
	dump(c);
	assert(foo && !c);
	assert(optind == 3);

	c = getopt_long_only(test_argc, test_argv, shortopts, longopts, NULL);
	dump(c);
	assert(bar && !c);
	assert(optind == 4);
}

int main() {
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test7();
	test8();
}
