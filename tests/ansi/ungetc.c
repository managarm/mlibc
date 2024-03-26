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
	for (int c = '0'; c <= '9'; c++) {
		assert(fgetc(f) == c);
		assert(ungetc(c, f) == c);
		assert(fgetc(f) == c);
	}
	assert(fgetc(f) == EOF);
	assert(ungetc(EOF, f) == EOF);

	// Even though the spec does not guarantee it, we should be able to
	// ungetc more than one character.
	assert(ungetc('x', f) == 'x');
	assert(ungetc('y', f) == 'y');
	assert(fgetc(f) == 'y');
	assert(fgetc(f) == 'x');

	// Seeking should discard the effects of ungetc.
	assert(ungetc('x', f) == 'x');
	rewind(f);

	// Test pushing back a different character
	for (int c = '0'; c <= '9'; c++) {
		assert(fgetc(f) == c);
		assert(ungetc(c - '0' + 'a', f) == c - '0' + 'a');
		assert(fgetc(f) == c - '0' + 'a');
	}

#ifndef USE_HOST_LIBC
	// Too many ungetcs should fail.
	int eof = 0;
	for (int i = 0; i < 100; i++) {
		if (ungetc('x', f) == EOF) {
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
