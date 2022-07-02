#include <stdio.h>
#include <assert.h>
#include <string.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "fopen-host-libc.tmp"
#else
#define TEST_FILE "fopen.tmp"
#endif

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
	assert(fread(buffer, 1, sizeof(str) - 1, file));
	assert(!strcmp(buffer, str));
	fclose(file);

	// Open the file in appending mode, append string 2 (minus the null terminator) to the file, flush and close.
	file = fopen(TEST_FILE, "a");
	fwrite(str2, 1, sizeof(str2) - 1, file);
	fflush(file);
	fclose(file);

	// Open the file for reading again, verify the contents, close the file and return.
	file = fopen(TEST_FILE, "r");
	assert(fread(buffer2, 1, sizeof(completestr) - 1, file));
	assert(!strcmp(buffer2, completestr));
	fclose(file);

	// Check that stdout, stdin and stderr can be closed by the application (issue #12).
	fclose(stdout);
	fclose(stdin);
	fclose(stderr);

	return 0;
}
