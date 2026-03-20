#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

int main() {
	const wchar_t *str = L"wcsrtombs";
	const wchar_t *p = str;
	size_t ret = wcsrtombs(NULL, &p, 343245234, NULL);
	assert(ret == 9);

	str = L"\xffffffff";
	p = str;
	ret = wcsrtombs(NULL, &p, 343245234, NULL);
	assert(ret == (size_t) -1);
	assert(errno == EILSEQ);

	mbstate_t ps = {0};
	char mbs[10] = "";
	str = L"wcsrtombs";
	p = str;
	ret = wcsrtombs(mbs, &p, 3, &ps);
	assert(ret == 3);
	assert(!strncmp(mbs, "wcs", 3));
	assert(p == &str[3]);

	memset(&ps, 0, sizeof(ps));
	p = str;
	ret = wcsrtombs(mbs, &p, 10, &ps);
	assert(ret == 9);
	assert(!strcmp(mbs, "wcsrtombs"));
	assert(!p);

	return 0;
}
