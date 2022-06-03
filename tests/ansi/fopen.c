#include <stdio.h>
#include <assert.h>
#include <string.h>

#define TEST_FILE "fopen.tmp"

int main() {
	FILE *file;
	char str[] = "mlibc fopen test";
	char str2[] = " mlibc appending";
	char completestr[] = "mlibc fopen test mlibc appending";
	char buffer[100];
	char buffer2[100];

	// Clear all the buffers to zero.
	memset(buffer, 0, sizeof(buffer));
	memset(buffer2, 0, sizeof(buffer2));

	// Open the file for writing.
	file = fopen(TEST_FILE, "w");
	assert(file);

	// Write string minus null terminator, flush and close.
	fwrite(str, 1, sizeof(str) - 1, file);
	fflush(file);
	fclose(file);

	// Open the file for reading.
	file = fopen(TEST_FILE, "r");
	assert(file);

	// Verify that we read back the written string and close the file.
	fread(buffer, strlen(str) + 1, 1, file);
	assert(!strcmp(buffer, str));
	fclose(file);

	// Open the file in appending mode, append string 2 (minus the null terminator) to the file, flush and close.
	file = fopen(TEST_FILE, "a");
	fwrite(str2, 1, sizeof(str2) - 1, file);
	fflush(file);
	fclose(file);

	// Open the file for reading again, verify the contents, close the file and return.
	file = fopen(TEST_FILE, "r");
	fread(buffer2, strlen(completestr) + 1, 1, file);
	assert(!strcmp(buffer2, completestr));
	fclose(file);

	return 0;
}
