#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

int main() {
	char *lang = setlocale(LC_ALL, "en_US.utf8");
	assert(lang);

	// ab✨⛔
	const char *p = "ab\xE2\x9C\xA8\xE2\x9B\x94";
	assert(mbsrtowcs(NULL, &p, 343245234, NULL) == 4);

	// truncated ✨
	p = "ab\xE2\x9C";
	assert(mbsrtowcs(NULL, &p, 343245234, NULL) == (size_t)-1);
	assert(errno == EILSEQ);

	mbstate_t state = { 0 };
	p = "ab";
	const char *src = p;
	assert(mbsrtowcs(NULL, &src, 1, &state) == 2);
	src = p + 1;
	assert(mbsrtowcs(NULL, &src, 1, &state) == 1);
	src = p + 2;
	assert(mbsrtowcs(NULL, &src, 1, &state) == 0);

	p = "";
	assert(mbsrtowcs(NULL, &p, 1, NULL) == 0);

	p = "\xE2\x9C";
	src = p;
	wchar_t wc;
	memset(&state, 0, sizeof(state));
	assert(mbsrtowcs(&wc, &src, 1, &state) == (size_t) -1);

	p = "\xE2\x9C\xA8\xE2\x9B\x94";
	src = p;
	wchar_t wc3[3];
	memset(wc3, 0x42, sizeof(wc3));
	memset(&state, 0, sizeof(state));
	assert(mbsrtowcs(wc3, &src, 1, &state) == 1);
	assert(src == &p[3]);
	assert(mbsrtowcs(wc3 + 1, &src, 1, &state) == 1);
	assert(src == &p[6]);
	assert(wc3[2] == 0x42424242);
	assert(mbsrtowcs(wc3 + 2, &src, 1, &state) == 0);
	assert(src == NULL);
	assert(wc3[0] == L'✨');
	assert(wc3[1] == L'⛔');
	assert(wc3[2] == L'\0');

	p = "\xC0\x81";
	src = p;
	memset(&state, 0, sizeof(state));
	assert(mbsrtowcs(wc3, &src, 3, &state) == (size_t) -1);

	p = "\xF1\x80\x80\x80";
	src = p;
	wc = 0;
	memset(&state, 0, sizeof(state));
	assert(mbsrtowcs(&wc, &src, 1, &state) == 1);
	assert(wc == 0x40000);

	p = "\xED\xA0\x80";
	src = p;
	wc = 0;
	memset(&state, 0, sizeof(state));
	assert(mbsrtowcs(&wc, &src, 1, &state) == (size_t) -1);
	assert(errno == EILSEQ);

	p = "\xF4\x90\x00\x00";
	src = p;
	wc = 0;
	memset(&state, 0, sizeof(state));
	assert(mbsrtowcs(&wc, &src, 1, &state) == (size_t) -1);
	assert(errno == EILSEQ);

	return 0;
}
