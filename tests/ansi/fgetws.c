#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <wchar.h>

int main() {
	char *locale = setlocale(LC_ALL, "en_US.utf8");
	assert(locale);

	FILE *fp = tmpfile();
	assert(fp);

	const wchar_t *test_string_1 = L"hello, world!";
	const wchar_t *test_string_2 = L"test αβγ \u2603";
	// fwprintf(fp, L"%ls\n%ls", test_string_1, test_string_2);
	fputws(test_string_1, fp);
	fputwc(L'\n', fp);
	fputws(test_string_2, fp);

	rewind(fp);

	wchar_t buffer[100];
	assert(fgetws(buffer, 100, fp) != NULL);
	assert(!wcsncmp(test_string_1, buffer, wcslen(test_string_1)));
	assert(fgetws(buffer, 100, fp) != NULL);
	assert(!wcsncmp(test_string_2, buffer, wcslen(test_string_2)));

	fclose(fp);

	return 0;
}
