#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <wordexp.h>

static const char *wordexp_return_stringify(int val, int *should_free);

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
	char *cwd = getcwd(NULL, 0);
	assert(!strcmp(cwd, we.we_wordv[0]));
	wordfree(&we);
	free(cwd);

	char *home;
	assert(asprintf(&home, "%s/.config", getenv("HOME")) != -1);
	wordexp("$HOME ~ ~/.config", &we, WRDE_REUSE);
	assert(!strcmp(getenv("HOME"), we.we_wordv[0]));
	assert(!strcmp(getenv("HOME"), we.we_wordv[1]));
	assert(!strcmp(home, we.we_wordv[2]));
	free(home);
	wordfree(&we);

	struct passwd *pw = getpwnam("root");
	assert(asprintf(&home, "%s/.config", pw->pw_dir) != -1);
	wordexp("~root ~root/.config", &we, WRDE_REUSE);
	assert(!strcmp(pw->pw_dir, we.we_wordv[0]));
	assert(!strcmp(home, we.we_wordv[1]));
	free(home);
	wordfree(&we);

	size_t i = 0;

	for(struct we_test *test = &test_cases[i]; test->expected_return != -1; test = &test_cases[++i]) {
		wordexp_t test_we;

		int should_free;
		const char *expected = wordexp_return_stringify(test->expected_return, &should_free);
		fprintf(stderr, "TESTCASE %zu: '%s' with flags %d expected to return %s\n", i, test->words, test->flags, expected);
		int test_ret = wordexp(test->words, &test_we, test->flags);

		if(test_ret != test->expected_return) {
			fprintf(stderr, "\twordexp() returned %s, but we expect %s\n", wordexp_return_stringify(test_ret, &should_free), wordexp_return_stringify(test->expected_return, &should_free));
		}

		assert(test_ret == test->expected_return);

		if(test_ret != 0)
			continue;

		assert(test_we.we_wordc == test->wordc);

		for(size_t j = 0; j < 16 && j < test->wordc; j++) {
			assert(test->wordv[j] && test_we.we_wordv[j]);
			assert(!strcmp(test->wordv[j], test_we.we_wordv[j]));
		}

		wordfree(&test_we);
		if (should_free)
			free((void *)expected);
	}

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

static const char *wordexp_return_stringify(int val, int *should_free) {
	for(size_t i = 0; wordexp_return_vals[i].val != -1 || wordexp_return_vals[i].string; i++) {
		if(wordexp_return_vals[i].val == val) {
			*should_free = 0;
			return wordexp_return_vals[i].string;
		}
	}

	char *unknown;
	assert(asprintf(&unknown, "unknown return value %d", val) != -1);
	*should_free = 1;

	return unknown;
}
