#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

void verify_decode(const char *input, wchar_t expected) {
	mbstate_t state;
	wchar_t wc;
	int n = strlen(input);
	int res;

	// Check that the conversion with mbrtowc() works.
	memset(&state, 0, sizeof(mbstate_t));
	res = mbrtowc(&wc, input, n, &state);

	if(res != n) {
		printf("mbtowc decoded %d bytes (expected %d bytes)\n", res, n);
		abort();
	}
	if(wc != expected) {
		printf("mbtowc returned cp %x (expected cp %x)\n", wc, expected);
		abort();
	}
}

int main() {
	verify_decode("\x24", 0x24);
	verify_decode("\xC2\xA2", 0xA2);
	verify_decode("\xE0\xA4\xB9", 0x939);
	verify_decode("\xE2\x82\xAC", 0x20AC);
	verify_decode("\xED\x95\x9C", 0xD55C);
	verify_decode("\xF0\x90\x8D\x88", 0x10348);
}
