#include <assert.h>
#include <stdio.h>
#include <string.h>

#if defined(USE_HOST_LIBC)
#define TEST_FILE "fseek-eof-host-libc.tmp"
#elif defined(USE_CROSS_LIBC)
#define TEST_FILE "fseek-eof-cross-libc.tmp"
#else
#define TEST_FILE "fseek-eof.tmp"
#endif

int main() {
	char str[] = "mlibc fseek eof test";
	size_t str_size = sizeof(str) - 1;
	char buffer[64];

	FILE *file = fopen(TEST_FILE, "wb");
	assert(file);
	assert(fwrite(str, 1, str_size, file) == str_size);
	fflush(file);
	fclose(file);

	file = fopen(TEST_FILE, "rb");
	assert(file);

	assert(fread(buffer, 1, sizeof(buffer), file) == str_size);
	assert(feof(file));

	assert(fseek(file, 0, SEEK_SET) == 0);
	assert(!feof(file));

	assert(fread(buffer, 1, 5, file) == 5);
	assert(!memcmp(buffer, "mlibc", 5));
	assert(!feof(file));

	assert(fread(buffer, 1, sizeof(buffer), file) == str_size - 5);
	assert(feof(file));
	rewind(file);
	assert(!feof(file));

	fclose(file);
	return 0;
}
