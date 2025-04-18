#include <stdio.h>
#include <wchar.h>
#include <limits.h>
#include <locale.h>
#include <assert.h>

int main() {
	wchar_t c = 0xC9;
	unsigned char buf[MB_LEN_MAX] = { 0 };
	setlocale(LC_ALL, "");
	if (sprintf(buf, "%lc", c) < 0)
		return -1;

	assert(buf[0] == 0xc3 && buf[1] == 0x89
			&& buf[2] == '\0' && buf[3] == '\0');

	return 0;
}
