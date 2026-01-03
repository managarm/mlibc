#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	memset(buf, 0x42, sizeof(buf));
	mbslen = mbstowcs(buf, "", 1);
	assert(mbslen == 0);
	assert(buf[0] == L'\0');
	assert(buf[1] == 0x42424242);

	// python's "checking" of the function for detecting locale charcode (lol)
	for (unsigned int i = 0x80; i <= 0xff; i++) {
		char ch[1];
		wchar_t wch[1];

		unsigned uch = (unsigned char)i;
		ch[0] = (char)uch;
		mbstowcs(wch, ch, 1);
	}

	// ✨
	memset(buf, 0x42, sizeof(buf));
	mbslen = mbstowcs(buf, "\xE2\x9C\xA8", 10);
	assert(mbslen == 1);
	assert(buf[0] == L'✨');
	assert(buf[1] == L'\0');
	assert(buf[2] == 0x42424242);

	// ✨⛔
	memset(buf, 0x42, sizeof(buf));
	mbslen = mbstowcs(buf, "\xE2\x9C\xA8\xE2\x9B\x94", 1);
	assert(mbslen == 1);
	assert(buf[0] == L'✨');
	assert(buf[1] == 0x42424242);

	// truncated ✨
	mbslen = mbstowcs(buf, "\xE2\x9C", 10);
	assert(mbslen == (size_t) -1);
	assert(errno == EILSEQ);

	memset(buf, 0x42, sizeof(buf));
	mbslen = mbstowcs(buf, "", 1);
	assert(mbslen == 0);
	assert(buf[0] == L'\0');

	return 0;
}
