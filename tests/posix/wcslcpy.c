#include <assert.h>
#include <wchar.h>

int main(void) {
	wchar_t buf1[6];
	size_t r1 = wcslcpy(buf1, L"Hello", 6);
	assert(r1 == 5);
	assert(wcscmp(buf1, L"Hello") == 0);

	wchar_t buf2[6];
	size_t r2 = wcslcpy(buf2, L"overlongstring", 6);
	assert(r2 == 14);
	assert(wcscmp(buf2, L"overl") == 0);

	wchar_t buf3[4] = L"ABC";
	size_t r3 = wcslcpy(buf3, L"Test", 0);
	assert(r3 == 4);
	assert(wcscmp(buf3, L"ABC") == 0);

	wchar_t buf4[10] = L"mlibc";
	size_t r4 = wcslcpy(buf4, L"", 10);
	assert(r4 == 0);
	assert(wcscmp(buf4, L"") == 0);

	return 0;
}
