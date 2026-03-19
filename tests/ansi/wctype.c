#include <assert.h>
#include <wctype.h>

int main(void) {
	wctype_t charclass = wctype("alpha");
	assert(charclass);

	int ret = iswctype(L'B', charclass);
	assert(ret);
	ret = iswctype(L'c', charclass);
	assert(ret);

	ret = iswctype(L'0', charclass);
	assert(!ret);
	ret = iswctype(L'-', charclass);
	assert(!ret);

	return 0;
}
