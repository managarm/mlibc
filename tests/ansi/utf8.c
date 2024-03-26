#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#if UINTPTR_MAX == UINT64_MAX
#define WCHAR_SPEC ""
#else
#define WCHAR_SPEC "l"
#endif

#define EXPECT(func) ({ \
		if(res != expected_ret) { \
			printf(#func " decoded %d bytes (expected %d bytes), at %s:%d\n", \
					res, expected_ret, __FILE__, line); \
			fflush(stdout); \
			abort(); \
		} \
		if(wc != expected) { \
			printf(#func " output cp %" WCHAR_SPEC "x (expected cp %" WCHAR_SPEC "x), at %s:%d\n", wc, \
					expected, __FILE__, line); \
			fflush(stdout); \
			abort(); \
		} \
	})

void verify_decode(const char *input, wchar_t expected, int line) {
	wchar_t wc;
	int bytes = *input ? strlen(input) : 1;
	int expected_ret = *input ? strlen(input) : 0;

	// Check mbrtowc().
	mbstate_t state;
	memset(&state, 0, sizeof(state));
	assert(mbrtowc(NULL, NULL, -1, &state) == 0);
	int res = mbrtowc(&wc, input, bytes, &state);
	EXPECT("mbrtowc");
	res = mbrtowc(NULL, input, bytes, &state);
	EXPECT("mbrtowc (pwc == NULL)");

	// Check mbtowc().
	assert(mbtowc(NULL, NULL, -1) == 0);
	res = mbtowc(&wc, input, bytes);
	EXPECT("mbtowc");
	res = mbtowc(NULL, input, bytes);
	EXPECT("mbtowc (pwc == NULL)");
}

int main() {
	setlocale(LC_ALL, "C.UTF-8");

#define verify_decode(in, out) verify_decode(in, out, __LINE__)
	verify_decode("\x24", 0x24);
	verify_decode("\xC2\xA2", 0xA2);
	verify_decode("\xE0\xA4\xB9", 0x939);
	verify_decode("\xE2\x82\xAC", 0x20AC);
	verify_decode("\xED\x95\x9C", 0xD55C);
	verify_decode("\xF0\x90\x8D\x88", 0x10348);
	verify_decode("", L'\0');

	return 0;
}
