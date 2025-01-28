#include <assert.h>
#include <locale.h>
#include <string.h>

int main() {
	setlocale(LC_ALL, "C");

	const char *buf = "cbtteststring";
	char dest[14];
	size_t ret = strxfrm(dest, buf, strlen(buf) + 1);
	assert(ret == 13);
	int cmp = strncmp(dest, buf, 13);
	assert(!cmp);

	return 0;
}
