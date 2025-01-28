#include <assert.h>
#include <wchar.h>

int main() {
	assert(!wcsncasecmp(L"foo", L"foo", 3));
	assert(!wcsncasecmp(L"foo", L"FOO", 3));
	assert(!wcsncasecmp(L"fooa", L"FOOB", 3));
	assert(wcsncasecmp(L"foo", L"bar", 3));
	assert(!wcsncasecmp(L"fooa", L"FOOA", 4));
	assert(!wcsncasecmp(L"123abc", L"123AbC", 6));

	return 0;
}
