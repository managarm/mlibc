#include <assert.h>
#include <wchar.h>

int main(void) {
	wchar_t buf1[10] = L"foo";
	size_t r1 = wcslcat(buf1, L"bar", 10);
	assert(r1 == 6);
	assert(wcscmp(buf1, L"foobar") == 0);

	wchar_t buf2[6] = L"foo";
	size_t r2 = wcslcat(buf2, L"barbaz", 6);
	assert(r2 == 9);
	assert(wcscmp(buf2, L"fooba") == 0);

	wchar_t buf3[4] = L"ABC";
	size_t r3 = wcslcat(buf3, L"DEF", 0);
	assert(r3 == 3);
	assert(wcscmp(buf3, L"ABC") == 0);

	wchar_t buf4[10] = L"ABCD";
	size_t r4 = wcslcat(buf4, L"EF", 2);
	assert(r4 == 4);
	assert(wcscmp(buf4, L"ABCD") == 0);

	wchar_t buf5[10] = L"";
	size_t r5 = wcslcat(buf5, L"test", 10);
	assert(r5 == 4);
	assert(wcscmp(buf5, L"test") == 0);

	return 0;
}
