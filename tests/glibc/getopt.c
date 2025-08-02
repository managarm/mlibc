#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define dump(x) fprintf(stderr, "getopt c '%c' (%d), optind %d ('%s'), optarg '%s', optopt '%c' (%d)\n", \
		(x), (x), optind, (size_t)optind >= COUNT_OF(test_argv) ? "out of range" : test_argv[optind], optarg, optopt, optopt);

#define dumpargv(argv) do { \
	fprintf(stderr, "args(%zu) {\n", COUNT_OF(argv)); \
	for (size_t i = 0; i < COUNT_OF(argv); i++) { \
		fprintf(stderr, "\targv[%zu] = '%s'\n", i, argv[i]); \
	} \
	fprintf(stderr, "}\n"); \
} while (0)

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
	opterr = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:", longopts, NULL);
	// Expected result: return '?', optopt is set to the offender
	assert(c == '?');
	assert(optopt == 'c');

	fputs("Situation: we pass a non-existant short option in argv, while passing a leading colon in optstring\n", stderr);
	optind = 0;

	c = getopt_long(COUNT_OF(test_argv), test_argv, ":ab:", longopts, NULL);
	// Expected result: return '?', optopt is set to the offender
	assert(c == '?');
	assert(optopt == 'c');

	fputs("Situation: we omit the required arg to a short option\n", stderr);
	optind = 0;

	c = getopt_long(COUNT_OF(test_argv), test_argv, "ab:c:", longopts, NULL);
	// Expected result: return '?', optopt is set to the offender
	assert(c == '?');
	assert(optopt == 'c');

	fputs("Situation: we omit the required arg to a short option, while passing a leading colon in optstring\n", stderr);
	optind = 0;

	c = getopt_long(COUNT_OF(test_argv), test_argv, ":ab:c:", longopts, NULL);
	// Expected result: return ':', optopt is set to the offender
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
	// Expected result:
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
	// Expected result:
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
	dumpargv(test_argv);
	assert(c == 'S');
	c = getopt_long(test_argc, test_argv, "Cc:hnST:v", longopts, NULL);
	dump(c);
	dumpargv(test_argv);
	assert(optind == 2);
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
	unsetenv("POSIXLY_CORRECT");
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

void test9() {
	char *test_argv[] = {
		"foo",
	};
	struct option opts[] = { {0} };
	optind = 0;
	int c = getopt_long(1, test_argv, "abc", opts, NULL);
	dump(c);
	assert(c == -1);
}

void test10() {
	char *test_argv[] = {
		"foo",
		"nonoption",
		"-a",
		"nonoption2",
		"nonoption3",
		"-b",
		"-c",
	};
	struct option opts[] = { {0} };
	optind = 2;
	dump(0);
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "abc", opts, NULL);
	dump(c);
	dumpargv(test_argv);
	assert(c == 'a');
	c = getopt_long(COUNT_OF(test_argv), test_argv, "abc", opts, NULL);
	dump(c);
	dumpargv(test_argv);
	assert(c == 'b');
	c = getopt_long(COUNT_OF(test_argv), test_argv, "abc", opts, NULL);
	dump(c);
	dumpargv(test_argv);
	assert(c == 'c');
	c = getopt_long(COUNT_OF(test_argv), test_argv, "abc", opts, NULL);
	dump(c);
	dumpargv(test_argv);
	assert(c == -1);

	assert(!strcmp(test_argv[0], "foo"));
	assert(!strcmp(test_argv[1], "nonoption"));
	assert(!strcmp(test_argv[2], "-a"));
	assert(!strcmp(test_argv[3], "-b"));
	assert(!strcmp(test_argv[4], "-c"));
	assert(!strcmp(test_argv[5], "nonoption2"));
	assert(!strcmp(test_argv[6], "nonoption3"));
}

void test11() {
	const struct option longopts[] = {
		{"op", optional_argument, NULL, 'o'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"--op",
		"--op=arg",
		"non-option",
		"--op"
	};

	fputs("Situation: optional argument to long option\n", stderr);

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "", longopts, NULL);
	assert(c == 'o');
	assert(optarg == NULL);

	c = getopt_long(COUNT_OF(test_argv), test_argv, "", longopts, NULL);
	assert(c == 'o');
	assert(!strcmp(optarg, "arg"));

	c = getopt_long(COUNT_OF(test_argv), test_argv, "", longopts, NULL);
	assert(c == 'o');
	assert(optarg == NULL);
}

void test12() {
	char *test_argv[] = {
		"dummy",
		"-a",
		"-b",
		"arg",
		"-carg",
		"arg",
	};

	fputs("Situation: optional argument to short option\n", stderr);

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "ab::c::", NULL, NULL);
	assert(c == 'a');
	assert(optarg == NULL);

	c = getopt_long(COUNT_OF(test_argv), test_argv, "ab::c::", NULL, NULL);
	assert(c == 'b');
	assert(!optarg);

	c = getopt_long(COUNT_OF(test_argv), test_argv, "ab::c::", NULL, NULL);
	assert(c == 'c');
	assert(!strcmp(optarg, "arg"));

	c = getopt_long(COUNT_OF(test_argv), test_argv, "ab::c::", NULL, NULL);
	assert(c == -1);
	assert(optind == 4);
	assert(!strcmp(test_argv[optind], "arg"));
}

void test13() {
	const struct option longopts[] = {
		{"ambiguous-opt", no_argument, NULL, 0},
		{"ambiguous-option", no_argument, NULL, 1},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"--ambiguous-o"
	};

	fputs("Situation: ambiguous long option\n", stderr);

	optind = 0;
	opterr = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "", longopts, NULL);
	assert(c == '?');
}

void test14() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-Wfoo=bar",
	};

	fputs("Situation: -W for long options\n", stderr);

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "W;", longopts, NULL);
	assert(c == 'f');
	assert(!strcmp(optarg, "bar"));
}

void test15() {
	char *test_argv[] = {
		"dummy",
		"-c",
		"-a",
		"--",
		"-b"
	};

	fputs("Situation: -- to stop option processing\n", stderr);

	optind = 0;
	int c = getopt(COUNT_OF(test_argv), test_argv, "+ab");
	assert(c == '?');
	assert(optind == 2);

	c = getopt(COUNT_OF(test_argv), test_argv, "+ab");
	assert(c == 'a');
	assert(optind == 3);

	c = getopt(COUNT_OF(test_argv), test_argv, "+ab");
	assert(c == -1);
	assert(optind == 4);
	assert(!strcmp(test_argv[optind], "-b"));
}

void test16() {
	const struct option longopts[] = {
		{"add", required_argument, NULL, 'a'},
		{"append", no_argument, NULL, 'p'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"--ad=foo",
		"--appe"
	};

	fputs("Situation: non-ambiguous long option prefix\n", stderr);

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "", longopts, NULL);
	assert(c == 'a');
	assert(!strcmp(optarg, "foo"));

	c = getopt_long(COUNT_OF(test_argv), test_argv, "", longopts, NULL);
	assert(c == 'p');
	assert(optarg == NULL);
}

void test17() {
	const struct option longopts[] = {
		{"foo", no_argument, NULL, 'l'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-foo"
	};

	fputs("Situation: getopt_long_only with what could be short opts or a long opt\n", stderr);

	// Case 1: long option "foo" exists, so "-foo" should be treated as "--foo"
	optind = 0;
	int c = getopt_long_only(COUNT_OF(test_argv), test_argv, "f:o", longopts, NULL);
	assert(c == 'l');

	// Case 2: long option "foo" does not exist.
	const struct option longopts2[] = {
		{"bar", no_argument, NULL, 'b'},
		{NULL, 0, NULL, 0}
	};
	optind = 0;
	c = getopt_long_only(COUNT_OF(test_argv), test_argv, "f:o", longopts2, NULL);
	assert(c == 'f');
	assert(!strcmp(optarg, "oo"));
}

void test18() {
	const struct option longopts[] = {
		{"op", optional_argument, NULL, 'o'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-Wop=val",
	};

	fputs("Situation: -W with a long option with an optional argument (with value)\n", stderr);

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "W;", longopts, NULL);
	assert(c == 'o');
	assert(!strcmp(optarg, "val"));

	char *test_argv2[] = {
		"dummy",
		"-Wop",
	};

	fputs("Situation: -W with a long option with an optional argument (no value)\n", stderr);

	optind = 0;
	c = getopt_long(COUNT_OF(test_argv2), test_argv2, "W;", longopts, NULL);
	assert(c == 'o');
	assert(optarg == NULL);
}

void test19() {
	char *test_argv[] = {
		"dummy",
		"-a",
		"non-option",
		"-b",
	};

	fputs("Situation: POSIXLY_CORRECT behavior with '+'\n", stderr);

	optind = 0;
	int c = getopt(COUNT_OF(test_argv), test_argv, "+ab");
	assert(c == 'a');

	c = getopt(COUNT_OF(test_argv), test_argv, "+ab");
	assert(c == -1);
	assert(optind == 2);
	assert(!strcmp(test_argv[optind], "non-option"));
}

void test20() {
	const struct option longopts[] = {
		{"option", required_argument, NULL, 'o'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"--option",
		"-x",
	};

	fputs("Situation: long option with argument that looks like an option\n", stderr);

	optind = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "x", longopts, NULL);
	assert(c == 'o');
	assert(!strcmp(optarg, "-x"));
	assert(optind == 3);
}

void test21() {
	const struct option longopts[] = {
		{"ambig-one", no_argument, NULL, '1'},
		{"ambig-two", no_argument, NULL, '2'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-ambig",
	};

	fputs("Situation: ambiguous long option with getopt_long_only\n", stderr);

	optind = 0;
	opterr = 0;
	int c = getopt_long_only(COUNT_OF(test_argv), test_argv, "", longopts, NULL);
	assert(c == '?');
}

void test22() {
	const struct option longopts[] = {
		{"foo", no_argument, NULL, 'f'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-Wbar",
	};

	fputs("Situation: -W with non-existent long option\n", stderr);

	optind = 0;
	opterr = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "W;", longopts, NULL);
	assert(c == '?');
	assert(optind == 2);
}

void test23() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-foo",
		"val1",
		"-foo=val2",
	};

	fputs("Situation: getopt_long_only with required argument\n", stderr);

	optind = 0;
	int c = getopt_long_only(COUNT_OF(test_argv), test_argv, "", longopts, NULL);
	assert(c == 'f');
	assert(!strcmp(optarg, "val1"));
	assert(optind == 3);

	c = getopt_long_only(COUNT_OF(test_argv), test_argv, "", longopts, NULL);
	assert(c == 'f');
	assert(!strcmp(optarg, "val2"));
	assert(optind == 4);
}

void test24() {
	char *test_argv[] = {
		"dummy",
		"-W",
	};

	fputs("Situation: -W as a short option, not for passing long options\n", stderr);

	optind = 0;
	int c = getopt(COUNT_OF(test_argv), test_argv, "W");
	assert(c == 'W');
	assert(optind == 2);
}

void test25() {
	const struct option longopts[] = {
		{"foo", required_argument, NULL, 'f'},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-Wfoo",
	};

	fputs("Situation: -W with missing argument for a long option, with ':' in optstring\n", stderr);

	optind = 0;
	opterr = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, ":W;", longopts, NULL);
	assert(c == ':');
	assert(optopt == 'f');
	assert(optind == 2);
}

void test26() {
	const struct option longopts[] = {
		{"foo", no_argument, NULL, 0},
		{"ambig-one", no_argument, NULL, 1},
		{"ambig-two", no_argument, NULL, 2},
		{NULL, 0, NULL, 0}
	};

	char *test_argv[] = {
		"dummy",
		"-W",
	};

	fputs("Situation: -W without an argument\n", stderr);

	optind = 0;
	opterr = 1;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, ":W;", longopts, NULL);
	assert(c == ':');
	assert(optopt == 'W');
	assert(optind == 2);
}

void test27() {
	char *test_argv[] = {
		"dummy",
		"-Wambig",
	};

	const struct option longopts[] = {
		{"foo", no_argument, NULL, 0},
		{"ambig-one", no_argument, NULL, 1},
		{"ambig-two", no_argument, NULL, 2},
		{NULL, 0, NULL, 0}
	};

	fputs("Situation: -W with an ambiguous long option\n", stderr);

	optind = 0;
	opterr = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "W;", longopts, NULL);
	assert(c == '?');
	assert(optind == 2);
}

void test28() {
	char *test_argv[] = {
		"dummy",
		"-Wfoo=bar",
		"-Wfoo=",
	};

	const struct option longopts[] = {
		{"foo", no_argument, NULL, 0},
		{"ambig-one", no_argument, NULL, 1},
		{"ambig-two", no_argument, NULL, 2},
		{NULL, 0, NULL, 0}
	};

	fputs("Situation: -W with an argument to a long option that does not take one\n", stderr);

	optind = 0;
	opterr = 0;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "W;", longopts, NULL);
	assert(c == '?');
	assert(optind == 2);

	c = getopt_long(COUNT_OF(test_argv), test_argv, "W;", longopts, NULL);
	assert(c == '?');
	assert(optind == 3);

	c = getopt_long(COUNT_OF(test_argv), test_argv, "W;", longopts, NULL);
	assert(c == -1);
}

void test29() {
	char *test_argv[] = {
		"dummy",
		"--a",
	};

	const struct option longopts[] = {
		{"load-credentials", no_argument, NULL, 0x103},
		{NULL, 0, NULL, 0},
	};

	optind = 0;
	opterr = 1;
	int c = getopt_long(COUNT_OF(test_argv), test_argv, "el:sSRp:m:t:Vh", longopts, NULL);
	dump(c);
	assert(c == '?');
	assert(optind == 2);
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
	test9();
	test10();
	test11();
	test12();
	test13();
	test14();
	test15();
	test16();
	test17();
	test18();
	test19();
	test20();
	test21();
	test22();
	test23();
	test24();
	test25();
	test26();
	test27();
	test28();
	test29();

	return 0;
}
