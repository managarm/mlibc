#include <string.h>
#include <assert.h>

int main(int argc, char *argv[]) {
	char str[] = "This is a sample string";
	char expstr[] = "sample";
	char *result = memmem(str, 23, expstr, 6);

	// The memmem() function returns a pointer to the beginning
	// of the substring
	assert(strncmp(result, "sample", 6) == 0);

	// , or NULL if the substring is not found.
	assert(memmem(str, 10, expstr, 2) == NULL);
	return 0;
}
