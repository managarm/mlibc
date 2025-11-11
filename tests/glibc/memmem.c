#include <string.h>
#include <assert.h>

int main() {
	char *haystack = "abc123\0x45";

	char *needle1 = "abc";
	void *rv = memmem(haystack, strlen(haystack), needle1, strlen(needle1));
	assert(rv == haystack);

	char *needle2 = "123";
	rv = memmem(haystack, strlen(haystack), needle2, strlen(needle2));
	assert(rv == haystack + 3);

	char *needle3 = "1234";
	rv = memmem(haystack, strlen(haystack), needle3, strlen(needle3));
	assert(rv == NULL);

	char *needle4 = "23\0x45";
	rv = memmem(haystack, 10, needle4, 6);
	assert(rv == haystack + 4);

	return 0;
}
