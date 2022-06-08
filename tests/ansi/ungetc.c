#include <stdio.h>
#include <assert.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "ungetc-host-libc.tmp"
#else
#define TEST_FILE "ungetc.tmp"
#endif

void test(int buffering) {
	FILE *f = fopen(TEST_FILE, "w");
	for(int c = '0'; c <= '9'; c++) {
		fputc(c, f);
	}
	fclose(f);

	f = fopen(TEST_FILE, "r");
	if (!buffering) {
		setbuf(f, NULL);
	}

	assert(ungetc('x', f) == 'x');
	assert(fgetc(f) == 'x');

	// Test pushing back the same character
	for(int c = '0'; c <= '9'; c++) {
		assert(fgetc(f) == c);
		assert(ungetc(c, f) == c);
		assert(fgetc(f) == c);
	}
	assert(fgetc(f) == EOF);
	assert(ungetc(EOF, f) == EOF);
	assert(ungetc('x', f) == 'x');

	// Seeking should discard the effects of ungetc.
	rewind(f);

	// Test pushing back a different character
	for(int c = '0'; c <= '9'; c++) {
		assert(fgetc(f) == c);
		assert(ungetc(c - '0' + 'a', f) == c - '0' + 'a');
		assert(fgetc(f) == c - '0' + 'a');
	}

	fclose(f);

	// TODO: Test with other operations, like fread.
}

int main() {
	test(1);
	test(0);
}
