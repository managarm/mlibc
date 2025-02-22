#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "fputs-host-libc.tmp"
#else
#define TEST_FILE "fputs.tmp"
#endif

int main() {
	FILE *file;
	char str[] = "mlibc fputs test";
	char buffer[100];

	// Clear the buffer to zero.
	memset(buffer, 0, sizeof(buffer));

	// Open the file for writing.
	file = fopen(TEST_FILE, "w");
	assert(file);

	// Verify that we can write an empty string.
	assert(fputs("", file) != EOF);

	// Write string, flush and close.
	assert(fputs(str, file) != EOF);
	fflush(file);
	fclose(file);

	// Open the file for reading.
	file = fopen(TEST_FILE, "r");
	assert(file);

	// Verify that we read back the written string and close.
	assert(fread(buffer, 1, sizeof(str) - 1, file));
	assert(!strcmp(buffer, str));
	fclose(file);

	return 0;
}
