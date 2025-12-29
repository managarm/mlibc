#include <assert.h>
#include <wchar.h>
#include <stdlib.h>
#include <errno.h>
#include <locale.h>

int main() {
	char *lang = setlocale(LC_ALL, "en_US.utf8");
	assert(lang);

	const wchar_t *p = L"abcde";
	assert(wcsnrtombs(NULL, &p, 3, 343245234, NULL) == 3);

	p = L"abcd\u03f4"; // abcdTHETA
	assert(wcsnrtombs(NULL, &p, 5, 343245234, NULL) == 6);

	p = L"ab\xffffffff";
	assert(wcsnrtombs(NULL, &p, 3, 343245234, NULL) == (size_t)-1);
	assert(errno == EILSEQ);
	return 0;
}
