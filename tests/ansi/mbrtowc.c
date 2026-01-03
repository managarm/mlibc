#include <assert.h>
#include <locale.h>
#include <stddef.h>
#include <wchar.h>

int main() {
	char *lang = setlocale(LC_ALL, "en_US.utf8");
	assert(lang);

	const char *p = "\xE2\x9C\xA8";
	wchar_t wc;
	assert(mbrtowc(&wc, p, 1, NULL) == ((size_t) -2));
	assert(mbrtowc(&wc, p + 1, 1, NULL) == ((size_t) -2));
	assert(mbrtowc(&wc, p + 2, 1, NULL) == 1);
	assert(wc == L'✨');

	mbstate_t state = { 0 };
	assert(mbrtowc(&wc, p, 3, &state) == 3);
	assert(mbsinit(&state));
	assert(wc == L'✨');
	assert(mbrtowc(&wc, p + 3, 1, &state) == 0);
	assert(mbsinit(&state));
	assert(wc == L'\0');

	return 0;
}
