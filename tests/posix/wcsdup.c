#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

int main() {
	wchar_t *wide_string = L"test string\n";
	wchar_t *dup_string = wcsdup(wide_string);
	assert(!memcmp(wide_string, dup_string, (wcslen(wide_string) + 1) * sizeof(wchar_t)));
	free(dup_string);

	return 0;
}
