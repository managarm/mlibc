#include <assert.h>
#include <stdio.h>
#include <wchar.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "ungetwc-host-libc.tmp"
#elif defined(USE_CROSS_LIBC)
#define TEST_FILE "ungetwc-cross-libc.tmp"
#else
#define TEST_FILE "ungetwc.tmp"
#endif

void test(int buffering) {
	FILE *f = fopen(TEST_FILE, "w");
	for(wint_t c = L'0'; c <= L'9'; c++) {
		fputwc(c, f);
	}
	fclose(f);

	f = fopen(TEST_FILE, "r");
	if (!buffering) {
		setbuf(f, NULL);
	}

	assert(ungetwc(L'x', f) == L'x');
	assert(fgetwc(f) == L'x');

	// Test pushing back the same character
	for (wint_t c = L'0'; c <= L'9'; c++) {
		assert(fgetwc(f) == c);
		assert(ungetwc(c, f) == c);
		assert(fgetwc(f) == c);
	}
	assert(fgetwc(f) == WEOF);
	assert(ungetwc(WEOF, f) == WEOF);

	// Even though the spec does not guarantee it, we should be able to
	// ungetwc more than one character.
	assert(ungetwc(L'x', f) == L'x');
	assert(ungetwc(L'y', f) == L'y');
	assert(fgetwc(f) == L'y');
	assert(fgetwc(f) == L'x');

	// Seeking should discard the effects of ungetwc.
	assert(ungetwc(L'x', f) == L'x');
	rewind(f);

	// Test pushing back a different character
	for (wint_t c = L'0'; c <= L'9'; c++) {
		assert(fgetwc(f) == c);
		assert(ungetwc(c - L'0' + L'a', f) == c - L'0' + L'a');
		assert(fgetwc(f) == c - L'0' + L'a');
	}

#if !defined(USE_HOST_LIBC) && !defined(USE_CROSS_LIBC)
	// Too many ungetcs should fail.
	int eof = 0;
	for (int i = 0; i < 100; i++) {
		if (ungetwc(L'x', f) == WEOF) {
			eof = 1;
			break;
		}
	}
	assert(eof);
#endif

	fclose(f);

	// TODO: Test with other operations, like fread.
}

int main() {
	fprintf(stderr, "with buffering...\n");
	test(1);
	fprintf(stderr, "without buffering...\n");
	test(0);

	return 0;
}
