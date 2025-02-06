#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "ftell-host-libc.tmp"
#else
#define TEST_FILE "ftell.tmp"
#endif

int main() {
	FILE *file;
	char str[] = "mlibc ftell test";
	size_t str_size = sizeof(str) - 1;
	char buffer[20];

	// Clear buffer to zero
	memset(buffer, 0, sizeof(buffer));

	// Open the file for writing in binary mode, because ftell is unspecified
	// in text mode.
	file = fopen(TEST_FILE, "wb");
	assert(file);

	// Write string minus null terminator, flush and close.
	assert(fwrite(str, 1, str_size, file) == str_size);
	fflush(file);
	fclose(file);

	// Open the file for reading in binary mode.
	file = fopen(TEST_FILE, "rb");
	assert(file);

	// Check position indicator at the start of the file.
	assert(ftell(file) == 0);

	// Read 4 bytes and check fread and ftell.
	assert(fread(buffer, 1, 4, file) == 4);
	assert(ftell(file) == 4);

	// Rewind and check position indicator at the start of the file.
	rewind(file);
	assert(ftell(file) == 0);

	// Read the entire file and check fread and ftell.
	assert(fread(buffer, 1, str_size, file) == str_size);
	assert((size_t) ftell(file) == str_size);

	// Rewind and check how ftell interacts with getc.
	rewind(file);
	assert(fgetc(file) == 'm');
	assert(ftell(file) == 1);
	assert(fgetc(file) == 'l');
	assert(ftell(file) == 2);
	assert(fgetc(file) == 'i');
	assert(ftell(file) == 3);

	// Check whether ftell is decremented after ungetc
	assert(ungetc('X', file) == 'X');
	int ftell_after_ungetc = ftell(file);
	fprintf(stderr, "ftell_after_ungetc: %d\n", ftell_after_ungetc);
	assert(ftell_after_ungetc == 2);
	ungetc('Y', file);
	assert(ftell(file) == 1);

	// Check if rewind undoes ungetc's effects on ftell
	rewind(file);
	assert(ftell(file) == 0);

	fclose(file);
	return 0;
}
