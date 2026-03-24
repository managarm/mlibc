#include <assert.h>
#include <wchar.h>

int main(void) {
	wchar_t buf1[20];
	wchar_t *r1 = wcpcpy(buf1, L"hello");
	assert(r1 == buf1 + 5);
	assert(*r1 == L'\0');
	assert(wcscmp(buf1, L"hello") == 0);

	wchar_t buf2[10] = L"mlibc";
	wchar_t *r2 = wcpcpy(buf2, L"");
	assert(r2 == buf2);
	assert(*r2 == L'\0');
	assert(wcscmp(buf2, L"") == 0);

	wchar_t buf3[20];
	wchar_t *p = buf3;
	p = wcpcpy(p, L"foo");
	p = wcpcpy(p, L"bar");
	p = wcpcpy(p, L"baz");
	assert(p == buf3 + 9);
	assert(*p == L'\0');
	assert(wcscmp(buf3, L"foobarbaz") == 0);

	wchar_t buf4[15];
	wchar_t *r4 = wcpcpy(buf4, L"A");
	assert(r4 == buf4 + 1);
	assert(*r4 == L'\0');
	assert(wcscmp(buf4, L"A") == 0);

	return 0;
}
