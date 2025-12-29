#include <assert.h>
#include <wchar.h>
#include <stdlib.h>
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
	return 0;
}
