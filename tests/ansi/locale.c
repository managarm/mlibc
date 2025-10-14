#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

int main() {
	char buf[64] = { 0 };
	wint_t c = 0xC9;
	setlocale(LC_ALL, "");
	if (sprintf(buf, "%lc", c) < 0)
		return -1;

	assert((unsigned char) buf[0] == 0xc3 && (unsigned char) buf[1] == 0x89
			&& buf[2] == '\0' && buf[3] == '\0');

	setlocale(LC_ALL, "C");

	snprintf(buf, sizeof(buf), "%2.2f", 12.34);
	assert(!strcmp("12.34", buf));

	setlocale(LC_ALL, "de_DE");

	snprintf(buf, sizeof(buf), "%2.2f", 12.34);
	assert(!strcmp("12,34", buf));

	char lower = tolower('A');
	assert(lower == 'a');
	char upper = toupper(lower);
	assert(upper == 'A');

	assert(isalpha('z'));
	assert(!isalpha('z' + 1));

	snprintf(buf, sizeof(buf), "%'g", 12345.67);
	// assert(!strcmp("12.345,7", buf));

	return 0;
}
