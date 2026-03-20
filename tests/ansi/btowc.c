#include <assert.h>
#include <stdio.h>
#include <wchar.h>

int main(void) {
	assert(btowc('A') == L'A');
    assert(btowc('z') == L'z');
    assert(btowc('9') == L'9');
    assert(btowc('\n') == L'\n');
    assert(btowc('\0') == L'\0');
    assert(btowc('\0') == L'\0');
    assert(btowc(0x80) == WEOF);
    assert(btowc(EOF) == WEOF);

	return 0;
}
