#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char *lang = setlocale(LC_ALL, "en_US.utf8");
	assert(lang);

	size_t mbslen = mbstowcs(NULL, "ßß", 0);
	assert(mbslen == 2);

	mbslen = mbstowcs(NULL, "text", 0);
	assert(mbslen == 4);

	mbslen = mbstowcs(NULL, "", 0);
	assert(mbslen == 0);

	wchar_t buf[10];
	mbslen = mbstowcs(buf, "", 1);
	assert(mbslen == 0);

	// python's "checking" of the function for detecting locale charcode (lol)
	for (unsigned int i = 0x80; i <= 0xff; i++) {
		char ch[1];
		wchar_t wch[1];

		unsigned uch = (unsigned char)i;
		ch[0] = (char)uch;
		mbstowcs(wch, ch, 1);
	}

	// ✨
	mbslen = mbstowcs(buf, "\xE2\x9C\xA8", 10);
	assert(mbslen == 1);

	// ✨⛔
	mbslen = mbstowcs(buf, "\xE2\x9C\xA8\xE2\x9B\x94", 1);
	assert(mbslen == 1);

	// truncated ✨
	mbslen = mbstowcs(buf, "\xE2\x9C", 10);
	assert(mbslen == (size_t) -1);
	assert(errno == EILSEQ);

	return 0;
}
