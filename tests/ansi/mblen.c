#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <stdlib.h>

int main(void) {
	char *lang = setlocale(LC_ALL, "en_US.utf8");
	assert(lang);

	// ✨
	assert(mblen("\xE2\x9C\xA8", 10) == 3);

	// ✨⛔
	assert(mblen("\xE2\x9C\xA8\xE2\x9B\x94", 10) == 3);

	// ✨⛔
	assert(mblen("\xE2\x9C\xA8\xE2\x9B\x94", 3) == 3);

	// truncated ✨
	assert(mblen("\xE2\x9C", 10) == -1);
	assert(errno == EILSEQ);

	assert(mblen("", 1) == 0);
	assert(mblen("\0a", 2) == 0);

	lang = setlocale(LC_ALL, "POSIX");
	assert(lang);

#if !USE_HOST_LIBC && !USE_CROSS_LIBC
	for (int i = 0; i <= UCHAR_MAX; i++) {
		char buf[2] = { (unsigned char) i, '\0' };
		int ret = mblen(buf, 2);
		assert(ret == 0 || ret == 1);
	}
#endif // !USE_HOST_LIBC && !USE_CROSS_LIBC

	return 0;
}
