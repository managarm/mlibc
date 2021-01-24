#define _GNU_SOURCE

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <wordexp.h>

static const char *wordexp_return_stringify(int val);

struct we_test {
	int expected_return;
	const char *words;
	int flags;
	const char *wordv[16];
	size_t wordc;
} test_cases[] = {
	/* (unbalanced) pairs */
	{WRDE_SYNTAX, "\\", 0, {NULL}, 0},
	{WRDE_SYNTAX, "\'", 0, {NULL}, 0},
	{0, "\'\'", 0, {""}, 1},
	{0, "\'\"\'", 0, {"\""}, 1},
	{WRDE_SYNTAX, "\"\'", 0, {NULL}, 0},

	/* numbers & calculations */
	{0, "$((5+5))", 0, {"10"}, 1},
	{0, "$((-5+5))", 0, {"0"}, 1},
	{0, "$((010))", 0, {"8"}, 1},
	{0, "$((0x10))", 0, {"16"}, 1},

	/* errant symbols */
	{WRDE_BADCHAR, "(string", WRDE_NOCMD, {NULL}, 0},
	{WRDE_BADCHAR, "string)", WRDE_NOCMD, {NULL}, 0},
	{WRDE_BADCHAR, "{string", WRDE_NOCMD, {NULL}, 0},
	{WRDE_BADCHAR, "string}", WRDE_NOCMD, {NULL}, 0},
	{WRDE_BADCHAR, "<string", WRDE_NOCMD, {NULL}, 0},
	{WRDE_BADCHAR, "string>", WRDE_NOCMD, {NULL}, 0},
	{WRDE_BADCHAR, "string|", WRDE_NOCMD, {NULL}, 0},
	{WRDE_BADCHAR, "string;", WRDE_NOCMD, {NULL}, 0},
	{WRDE_BADCHAR, "string&", WRDE_NOCMD, {NULL}, 0},

	/* command substitution with WRDE_NOCMD */
	{WRDE_CMDSUB, "$(pwd)", WRDE_NOCMD, {NULL}, 0},

	/* env vars */
	{WRDE_BADVAL, "$DOES_NOT_EXIST", WRDE_UNDEF, {NULL}, 0},

	/* normal arguments */
	{0, "arg1 arg2", 0, {"arg1", "arg2"}, 2},
	{-1, NULL, 0, {NULL}, 0},
};

int main() {
	wordexp_t we;

	wordexp("$(pwd)", &we, 0);
	assert(!strcmp(getcwd(NULL, 0), we.we_wordv[0]));

	char *home;
	asprintf(&home, "%s/.config", getenv("HOME"));
	wordexp("$HOME ~ ~/.config", &we, WRDE_REUSE);
	assert(!strcmp(getenv("HOME"), we.we_wordv[0]));
	assert(!strcmp(getenv("HOME"), we.we_wordv[1]));
	assert(!strcmp(home, we.we_wordv[2]));
	free(home);

	struct passwd *pw = getpwnam("root");
	asprintf(&home, "%s/.config", pw->pw_dir);
	wordexp("~root ~root/.config", &we, WRDE_REUSE);
	assert(!strcmp(pw->pw_dir, we.we_wordv[0]));
	assert(!strcmp(home, we.we_wordv[1]));

	size_t i = 0;

	for(struct we_test *test = &test_cases[i]; test->expected_return != -1; test = &test_cases[++i]) {
		wordexp_t test_we;

		fprintf(stderr, "TESTCASE %zu: '%s' with flags %d expected to return %s\n", i, test->words, test->flags, wordexp_return_stringify(test->expected_return));
		int test_ret = wordexp(test->words, &test_we, test->flags);

		if(test_ret != test->expected_return) {
			fprintf(stderr, "\twordexp() returned %s, but we expect %s\n", wordexp_return_stringify(test_ret), wordexp_return_stringify(test->expected_return));
		}

		assert(test_ret == test->expected_return);

		if(test_ret != 0)
			continue;

		assert(test_we.we_wordc == test->wordc);

		for(size_t j = 0; j < 16 && j < test->wordc; j++) {
			assert(test->wordv[j] && test_we.we_wordv[j]);
			assert(!strcmp(test->wordv[j], test_we.we_wordv[j]));
		}
	}

	wordfree(&we);
	exit(EXIT_SUCCESS);
}

struct wordexp_return_val {
	int val;
	const char *string;
} wordexp_return_vals[] = {
	{0, "WRDE_SUCCESS"},
	{WRDE_BADCHAR, "WRDE_BADCHAR"},
	{WRDE_BADVAL, "WRDE_BADVAL"},
	{WRDE_CMDSUB, "WRDE_CMDSUB"},
	{WRDE_SYNTAX, "WRDE_SYNTAX"},
	{-1, NULL},
};

static const char *wordexp_return_stringify(int val) {
	for(size_t i = 0; wordexp_return_vals[i].val != -1 || wordexp_return_vals[i].string; i++) {
		if(wordexp_return_vals[i].val == val) {
			return wordexp_return_vals[i].string;
		}
	}

	char *unknown;
	asprintf(&unknown, "unknown return value %d", val);

	return unknown;
}
