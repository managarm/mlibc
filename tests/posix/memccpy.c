#include <assert.h>
#include <string.h>

int main() {
	const char *src = "Hello, World!";
	char dest[50];

	memset(dest, 'X', sizeof(dest));
	dest[49] = '\0';

	void *ret = memccpy(dest, src, 'o', 20);
	assert(memcmp(dest, "Hello", 5) == 0);
	assert(dest[5] == 'X');
	assert(ret == (dest + 5));

	memset(dest, 'X', sizeof(dest));
	ret = memccpy(dest, src, 'Z', 5);
	assert(memcmp(dest, "Hello", 5) == 0);
	assert(dest[5] == 'X');
	assert(ret == NULL);

	memset(dest, 'X', sizeof(dest));
	ret = memccpy(dest, src, 'o', 5);
	assert(memcmp(dest, "Hello", 5) == 0);
	assert(ret == (dest + 5));

	return 0;
}
