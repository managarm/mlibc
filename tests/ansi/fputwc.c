#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

int main() {
	char *locale = setlocale(LC_ALL, "en_US.utf8");
	assert(locale);
	FILE *fp = tmpfile();
	assert(fp);

	wint_t char_ascii = fputwc(L'A', fp);
	assert(char_ascii == L'A');
	wint_t char_euro = fputwc(L'€', fp);
	assert(char_euro == L'€');
	wint_t char_snowman = fputwc(L'\u2603', fp);
	assert(char_snowman == L'\u2603');

	assert(fwide(fp, 0) > 0);

	int ret = fseek(fp, 0, SEEK_SET);
	assert(ret == 0);

	wint_t wint = fgetwc(fp);
	assert(wint == L'A');

	wint = fgetwc(fp);
	fprintf(stderr, "wint = %x errno = %d (%s)\n", wint, errno, strerror(errno));
	assert(wint == L'€');

	wint = fgetwc(fp);
	fprintf(stderr, "wint = %x errno = %d (%s)\n", wint, errno, strerror(errno));
	assert(wint == L'\u2603');

	wint = fgetwc(fp);
	fprintf(stderr, "wint = %x errno = %d (%s)\n", wint, errno, strerror(errno));
	assert(wint == WEOF);
	assert(feof(fp));

	fp = freopen(NULL, "r+", fp);
	assert(fp);

	ret = fseek(fp, 0, SEEK_SET);
	assert(ret == 0);

	unsigned char buffer[10];
	size_t bytes_read = fread(buffer, 1, 10, fp);

	assert(bytes_read == 7);
	assert(buffer[0] == 0x41 && buffer[1] == 0xE2 && buffer[2] == 0x82 && buffer[3] == 0xAC);
	assert(buffer[4] == 0xE2 && buffer[5] == 0x98 && buffer[6] == 0x83);

	fclose(fp);

	return 0;
}
