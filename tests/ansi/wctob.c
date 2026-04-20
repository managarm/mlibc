#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <wchar.h>

int main(void) {
	char *loc = setlocale(LC_ALL, "C");
	assert(loc != NULL);

	errno = 0;

	int res = wctob((wint_t)L'A');
	assert(res == 'A');
	assert(errno == 0);

	res = wctob((wint_t)L'7');
	assert(res == '7');
	assert(errno == 0);

	res = wctob((wint_t)L'\n');
	assert(res == '\n');

	res = wctob((wint_t)L'\0');
	assert(res == '\0');

	res = wctob(WEOF);
	assert(res == EOF);

	res = wctob((wint_t)L'π');
	assert(res == EOF);

	return 0;
}
