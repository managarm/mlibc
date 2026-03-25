#include <assert.h>
#include <wchar.h>

int main(void) {
	wchar_t buf1[8] = {L'X', L'X', L'X', L'X', L'X', L'X', L'X', L'X'};
	wchar_t *r1 = wcpncpy(buf1, L"hello", 8);
	assert(r1 == buf1 + 5);
	assert(buf1[0] == L'h' && buf1[4] == L'o');
	assert(buf1[5] == L'\0');
	assert(buf1[6] == L'\0');
	assert(buf1[7] == L'\0');

	wchar_t buf2[6] = {L'X', L'X', L'X', L'X', L'X', L'\0'};
	wchar_t *r2 = wcpncpy(buf2, L"hello", 5);
	assert(r2 == buf2 + 5);
	assert(buf2[0] == L'h' && buf2[4] == L'o');
	assert(buf2[5] == L'\0');

	wchar_t buf3[6] = {L'X', L'X', L'X', L'X', L'X', L'\0'};
	wchar_t *r3 = wcpncpy(buf3, L"managarm", 4);
	assert(r3 == buf3 + 4);
	assert(buf3[0] == L'm' && buf3[3] == L'a');
	assert(buf3[4] == L'X');
	assert(buf3[5] == L'\0');

	wchar_t buf4[5] = {L'X', L'X', L'X', L'X', L'X'};
	wchar_t *r4 = wcpncpy(buf4, L"", 5);
	assert(r4 == buf4);
	assert(buf4[0] == L'\0');
	assert(buf4[1] == L'\0');
	assert(buf4[4] == L'\0');

	wchar_t buf5[3] = {L'A', L'B', L'C'};
	wchar_t *r5 = wcpncpy(buf5, L"test", 0);
	assert(r5 == buf5);
	assert(buf5[0] == L'A');
	assert(buf5[1] == L'B');
	assert(buf5[2] == L'C');

	return 0;
}
