#include <assert.h>
#include <string.h>
#include <stddef.h>

static char buf1[4096];
static char buf2[4096];

typedef int (*strcmp_func)(const char*, const char*);

struct strcmp_impl {
	const char *name;
	strcmp_func func;
};

#if !USE_HOST_LIBC && !USE_CROSS_LIBC
int __mlibc_strcmp_default(const char *s1, const char *s2);
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC

static struct strcmp_impl impls[] = {
	{"libc-provided strcmp", strcmp},

#if !USE_HOST_LIBC && !USE_CROSS_LIBC
	{"mlibc default fallback", __mlibc_strcmp_default},
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC
};

const size_t num_impls = sizeof(impls) / sizeof(impls[0]);

struct test_case {
	const char *s1;
	const char *s2;
	int expected_sign;
};

static struct test_case test_cases[] = {
	{"", "", 0},
	{"a", "", 1},
	{"", "a", -1},
	{"a", "a", 0},
	{"a", "b", -1},
	{"b", "a", 1},
	{"abc", "abc", 0},
	{"abc", "abd", -1},
	{"abd", "abc", 1},
	{"abc", "abcde", -1},
	{"abcde", "abc", 1},
	{"\xff", "\x01", 1},
	{"\x01", "\xff", -1},
	{"\x80", "\x7f", 1},
	{"\x7f", "\x80", -1},
};

static void verify_strcmp(const struct strcmp_impl *impl, const char *s1, const char *s2, int expected_sign) {
	int res = impl->func(s1, s2);
	if (expected_sign == 0) {
		assert(res == 0);
	} else if (expected_sign < 0) {
		assert(res < 0);
	} else {
		assert(res > 0);
	}
}

static void run_tests_for_impl(const struct strcmp_impl *impl) {
	for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++)
		verify_strcmp(impl, test_cases[i].s1, test_cases[i].s2, test_cases[i].expected_sign);

	memset(buf1, 'A', 2000);
	buf1[2000] = '\0';
	memset(buf2, 'A', 2000);
	buf2[2000] = '\0';

	verify_strcmp(impl, buf1, buf2, 0);

	buf1[999] = 'B';
	verify_strcmp(impl, buf1, buf2, 1);
	buf2[999] = 'C';
	verify_strcmp(impl, buf1, buf2, -1);
	buf1[999] = 'A';
	buf2[999] = 'A';

	buf1[1999] = 'B';
	verify_strcmp(impl, buf1, buf2, 1);
	buf1[1999] = 'A';

	strcpy(buf1 + 1, "test_string");
	strcpy(buf2 + 7, "test_string");
	verify_strcmp(impl, buf1 + 1, buf2 + 7, 0);

	strcpy(buf2 + 7, "test_strIng");
	verify_strcmp(impl, buf1 + 1, buf2 + 7, 1);
}

int main(void) {
	for (size_t k = 0; k < num_impls; k++)
		run_tests_for_impl(&impls[k]);

	return 0;
}
