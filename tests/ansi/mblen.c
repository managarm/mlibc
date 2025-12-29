#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <stdlib.h>

int main(void) {
	char *lang = setlocale(LC_ALL, "en_US.utf8");
	assert(lang);

	// ✨
	assert(mblen("\xE2\x9C\xA8", 10) == 3);

	// ✨⛔
	assert(mblen("\xE2\x9C\xA8\xE2\x9B\x94", 10) == 3);

	// truncated ✨
	assert(mblen("\xE2\x9C", 10) == -1);
	assert(errno == EILSEQ);
}
