#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <string.h>
#include <wchar.h>

int main(void) {
	char *lang = setlocale(LC_ALL, "en_US.utf8");
	assert(lang);

	assert(mbrlen("w", 10, NULL) == 1);

	// ✨
	assert(mbrlen("\xE2\x9C\xA8", 10, NULL) == 3);

	// ✨⛔
	assert(mbrlen("\xE2\x9C\xA8\xE2\x9B\x94", 10, NULL) == 3);

	// truncated ✨
	mbstate_t state = { 0 };
	assert(mbrlen("\xE2\x9C", 10, &state) == (size_t)-1);
	assert(errno == EILSEQ);
	memset(&state, 0, sizeof(state));
	assert(mbrlen("\xE2\x9C", 2, NULL) == (size_t)-2);
	assert(errno == EILSEQ);

	memset(&state, 0, sizeof(state));
	assert(mbrlen("", 1, &state) == 0);

	return 0;
}
