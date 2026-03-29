#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <unistd.h>
#include <wchar.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "getwchar-host-libc.tmp"
#elif defined(USE_CROSS_LIBC)
#define TEST_FILE "getwchar-cross-libc.tmp"
#else
#define TEST_FILE "getwchar.tmp"
#endif

int main() {
	char *l = setlocale(LC_ALL, "en_US.utf8");
	assert(l);

	FILE *fp = fopen(TEST_FILE, "w");
	assert(fp);

	// 'A', alpha (U+03B1), grinning face (U+1F600)
	fputs("A\xCE\xB1\xF0\x9F\x98\x80", fp);
	fclose(fp);

	fp = freopen(TEST_FILE, "r", stdin);
	assert(fp);

	int ret = fwide(stdin, 0);
	assert(ret == 0);

	wint_t wc = getwchar();
	assert(wc == L'A');

	ret = fwide(stdin, 0);
	assert(ret > 0);

	wc = getwchar();
	assert(wc == 0x03B1); // α

	wc = getwchar();
	assert(wc == 0x1F600); // 😀

	wc = getwchar();
	assert(wc == WEOF);

	unlink(TEST_FILE);
	return 0;
}
