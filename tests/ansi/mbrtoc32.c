#include <assert.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <uchar.h>

struct {
	size_t ret;
	char32_t c32;
	uint8_t bytes[4];
} expected_results[] = {
	{1, 0x7A, {0x7A, 0, 0, 0}},
	{2, 0xDF, {0xC3, 0x9F, 0, 0}},
	{3, 0x6C34, {0xE6, 0xB0, 0xB4, 0}},
	{4, 0x1F34C, {0xF0, 0x9F, 0x8D, 0x8C}},
};

int main() {
	setlocale(LC_ALL, "en_US.utf8");

	size_t ret = 0;

	char *str = "z\u00df\u6c34\U0001F34C";

	fprintf(stderr, "string: '%s'\n", str);

	mbstate_t state = {};
	char32_t c32;
	size_t loop = 0;
	while((ret = mbrtoc32(&c32, str, strlen(str), &state))) {
		assert(ret != (size_t)-3);
		if(ret == (size_t)-1)
			break;
		if(ret == (size_t)-2)
			break;

		fprintf(stderr, "Next UTF-32 char: 0x%x obtained from %zu bytes [", c32, ret);
		for(size_t n = 0; n < ret; ++n) {
			fprintf(stderr, " 0x%02x ", (uint8_t) str[n]);
		}
		fprintf(stderr, "]\n");

		assert(ret == expected_results[loop].ret);
		assert(c32 == expected_results[loop].c32);
		for(size_t n = 0; n < ret; ++n) {
			assert((uint8_t) str[n] == expected_results[loop].bytes[n]);
		}

		str += ret;
		loop++;
	}

	return 0;
}
