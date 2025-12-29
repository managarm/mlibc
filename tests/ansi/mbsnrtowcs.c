#include <assert.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <locale.h>

int main() {
	char *lang = setlocale(LC_ALL, "en_US.utf8");
	assert(lang);

	// ab✨⛔
	const char *p = "ab\xE2\x9C\xA8\xE2\x9B\x94";
	assert(mbsnrtowcs(NULL, &p, 5, 343245234, NULL) == 3);

	// truncated ✨
	p = "ab\xE2\x9C";
	assert(mbsnrtowcs(NULL, &p, 4, 343245234, NULL) == (size_t)2);

	p = "ab\xE2";
	assert(mbsnrtowcs(NULL, &p, 4, 343245234, NULL) == (size_t)-1);
	assert(errno == EILSEQ);

	p = "ab";
	const char *src = p;
	mbstate_t state = {};
	assert(mbsnrtowcs(NULL, &src, 1, 1, &state) == 1);
	src = p + 1;
	assert(mbsnrtowcs(NULL, &src, 1, 1, &state) == 1);
	src = p + 2;
	assert(mbsnrtowcs(NULL, &src, 1, 1, &state) == 0);

	// flush the error state
	p = "";
	assert(mbsnrtowcs(NULL, &p, 1, 0, NULL) == 0);

	p = "\xE2\x9C\xA8";
	wchar_t wc;
	src = p;
	assert(mbsnrtowcs(&wc, &src, 1, 1, NULL) == 0);
	assert(src == &p[1]);
	assert(mbsnrtowcs(&wc, &src, 1, 1, NULL) == 0);
	assert(src == &p[2]);
	assert(mbsnrtowcs(&wc, &src, 1, 1, NULL) == 1);
	assert(wc == L'✨');

	p = "\xE2\x9C\xA8\xE2\x9B\x94";
	src = p;
	wchar_t wc3[3];
	assert(mbsnrtowcs(wc3, &src, 1, 2, NULL) == 0);
	assert(src == &p[1]);
	assert(mbsnrtowcs(wc3, &src, 1, 2, NULL) == 0);
	assert(src == &p[2]);
	assert(mbsnrtowcs(wc3, &src, 3, 2, NULL) == 1);
	assert(src == &p[5]);
	assert(mbsnrtowcs(wc3 + 1, &src, 2, 1, NULL) == 1);
	assert(src == &p[6]);
	assert(mbsnrtowcs(wc3 + 2, &src, 1, 1, NULL) == 0);
	assert(src == NULL);
	assert(wc3[0] == L'✨');
	assert(wc3[1] == L'⛔');

	p = "\xE2\x9C\xA8\xE2\x9B\x94";
	src = p;
	assert(mbsnrtowcs(wc3, &src, 1, 2, NULL) == 0);
	assert(src == &p[1]);
	assert(mbsnrtowcs(wc3, &src, 2, 2, NULL) == 1);
	assert(src == &p[3]);
	assert(mbsnrtowcs(wc3 + 1, &src, 4, 2, NULL) == 1);
	assert(src == NULL);
	assert(wc3[0] == L'✨');
	assert(wc3[1] == L'⛔');
	assert(wc3[2] == L'\0');

	p = "\xE2\x9C\xA8";
	src = p;
	memset(&state, 0, sizeof(state));
	memset(wc3, 0x42, sizeof(wc3));
	assert(mbsnrtowcs(wc3, &src, 1, 1, &state) == 0);
	assert(src == &p[1]);
	assert(mbsnrtowcs(wc3, &src, 1, 1, &state) == 0);
	assert(src == &p[2]);
	assert(mbsnrtowcs(wc3, &src, 1, 1, &state) == 1);
	assert(wc3[0] == L'✨');
	assert(wc3[1] == 0x42424242);

	p = "\xE2\x9C\xA8\xE2\x9B\x94";
	src = p;
	memset(&state, 0, sizeof(state));
	assert(mbsnrtowcs(wc3, &src, 1, 2, &state) == 0);
	assert(src == &p[1]);
	assert(mbsnrtowcs(wc3, &src, 1, 2, &state) == 0);
	assert(src == &p[2]);
	assert(mbsnrtowcs(wc3, &src, 3, 2, &state) == 1);
	assert(src == &p[5]);
	assert(mbsnrtowcs(wc3 + 1, &src, 2, 1, &state) == 1);
	assert(src == &p[6]);
	assert(mbsnrtowcs(wc3 + 2, &src, 1, 1, &state) == 0);
	assert(src == NULL);
	assert(wc3[0] == L'✨');
	assert(wc3[1] == L'⛔');
	assert(wc3[2] == L'\0');
	assert(mbsinit(&state));

	p = "\xE2\x9C\xA8\xE2\x9B\x94";
	src = p;
	memset(&state, 0, sizeof(state));
	assert(mbsnrtowcs(wc3, &src, 1, 2, &state) == 0);
	assert(src == &p[1]);
	assert(mbsnrtowcs(wc3, &src, 2, 2, &state) == 1);
	assert(src == &p[3]);
	assert(mbsnrtowcs(wc3 + 1, &src, 4, 2, &state) == 1);
	assert(src == NULL);
	assert(wc3[0] == L'✨');
	assert(wc3[1] == L'⛔');
	assert(wc3[2] == L'\0');
	assert(mbsinit(&state));

	return 0;
}
