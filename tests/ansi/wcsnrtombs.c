#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

int main() {
	char *lang = setlocale(LC_ALL, "en_US.utf8");
	assert(lang);

	const wchar_t *wcs = L"abcde";
	const wchar_t *p = wcs;
	size_t ret = wcsnrtombs(NULL, &p, 3, 343245234, NULL);
	assert(ret == 3);

	wcs = L"abcd\u03f4"; // abcdTHETA
	p = wcs;
	ret = wcsnrtombs(NULL, &p, 5, 343245234, NULL);
	assert(ret == 6);

	wcs = L"ab\xffffffff";
	p = wcs;
	ret = wcsnrtombs(NULL, &p, 3, 343245234, NULL);
	assert(ret == (size_t)-1);
	assert(errno == EILSEQ);

	mbstate_t ps = {0};
	char mbs[6] = "";
	wcs = L"abcde";
	p = wcs;
	ret = wcsnrtombs(mbs, &p, 6, 4, &ps);
	assert(ret == 4);
	assert(!strcmp(mbs, "abcd"));
	assert(p == &wcs[4]);

	memset(&ps, 0, sizeof(ps));
	p = wcs;
	ret = wcsnrtombs(mbs, &p, 6, 5, &ps);
	assert(ret == 5);
	assert(!strcmp(mbs, "abcde"));
	assert(p == &wcs[5]);

	memset(&ps, 0, sizeof(ps));
	p = wcs;
	ret = wcsnrtombs(mbs, &p, 6, 6, &ps);
	assert(ret == 5);
	assert(!strcmp(mbs, "abcde"));
	assert(!p);

	return 0;
}
