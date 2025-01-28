#include <assert.h>
#include <locale.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#if UINTPTR_MAX == UINT64_MAX
#define WCHAR_SPEC ""
#else
#define WCHAR_SPEC "l"
#endif

/*
 * The code in this test is taken from https://github.com/termux/wcwidth/,
 * under the following license:
 *
 * Copyright (C) Fredrik Fornwall 2016.
 * Distributed under the MIT License.
 *
 * Implementation of wcwidth(3) as a C port of:
 * https://github.com/jquast/wcwidth
 *
 * Report issues at:
 * https://github.com/termux/wcwidth
 */

static int tests_run;
static int test_failures;

void assertWidthIs(int expected_width, wchar_t c) {
	tests_run++;
	int actual_width = wcwidth(c);
	if (actual_width != expected_width) {
		fprintf(stderr, "ERROR: wcwidth(U+%04" WCHAR_SPEC "x, '%lc') returned %d, expected %d\n", c, (wint_t) c, actual_width, expected_width);
		test_failures++;
	}
}

int main() {
	setlocale(LC_CTYPE, "C.UTF-8");
	assertWidthIs(1, 'a');
	assertWidthIs(1, L'ö');

	// Some wide:
	assertWidthIs(2, L'Ａ');
	assertWidthIs(2, L'Ｂ');
	assertWidthIs(2, L'Ｃ');
	assertWidthIs(2, L'中');
	assertWidthIs(2, L'文');
	assertWidthIs(2, 0x679C);
	assertWidthIs(2, 0x679D);
	assertWidthIs(2, 0x2070E);
	assertWidthIs(2, 0x20731);

#ifndef USE_HOST_LIBC
	assertWidthIs(1, 0x11A3);
#endif

	assertWidthIs(2, 0x1F428); // Koala emoji.
	assertWidthIs(2, 0x231a);  // Watch emoji.

	if (test_failures > 0) printf("%d tests FAILED, ", test_failures);
	printf("%d tests OK\n", tests_run - test_failures);
	return (test_failures == 0) ? 0 : 1;
}
