#include <assert.h>
#include <wchar.h>
#include <wctype.h>

int main(void) {
	wctrans_t desc = wctrans("tolower");
	assert(desc);

	wchar_t wc = towctrans(L'X', desc);
	assert(wc == L'x');

	return 0;
}
