#include <assert.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>
#include <wchar.h>

int main(void) {
	assert(btowc('A') == L'A');
    assert(btowc('z') == L'z');
    assert(btowc('9') == L'9');
    assert(btowc('\n') == L'\n');
    assert(btowc('\0') == L'\0');
    assert(btowc('\0') == L'\0');

#if !USE_HOST_LIBC && !USE_CROSS_LIBC
    assert(btowc(0x80) != WEOF);
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC

    assert(btowc(EOF) == WEOF);

    char *lang = setlocale(LC_ALL, "POSIX");
	assert(lang);

#if !USE_HOST_LIBC && !USE_CROSS_LIBC
	for (int i = 0; i <= 255; i++) {
		wint_t ret = btowc(i);
        // In the POSIX locale, btowc() shall not return WEOF if c has a value in the range 0 to 255 inclusive.
		assert(ret != WEOF);
	}
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC

	return 0;
}
