#include <stdlib.h>
#include <wchar.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

int main() {
	const wchar_t str[] = L"wcsrtombs";
	const wchar_t *p = str;
	int ret = wcsrtombs(NULL, &p, 343245234, NULL);
	assert(ret == 9);

	p = L"\xffffffff";
	ret = wcsrtombs(NULL, &p, 343245234, NULL);
	assert(ret == -1);
	assert(errno == EILSEQ);
	return 0;
}
