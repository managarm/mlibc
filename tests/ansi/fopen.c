#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "fopen-host-libc.tmp"
#elif defined(USE_CROSS_LIBC)
#define TEST_FILE "fopen-cross-libc.tmp"
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

	// Writing to a stream that was not opened for writing must fail even though the
	// write would otherwise just land in the buffer.
	file = fopen(TEST_FILE, "r");
	assert(file);
	errno = 0;
	assert(fwrite(str, 1, sizeof(str) - 1, file) == 0);
	assert(errno == EBADF);
	assert(ferror(file));
	clearerr(file);
	errno = 0;
	assert(fputc('x', file) == EOF);
	assert(errno == EBADF);
	assert(ferror(file));
	fclose(file);

	// Likewise, reading from a stream that was not opened for reading fails and,
	// unlike a short read, must not set the EOF indicator.
	file = fopen(TEST_FILE, "w");
	assert(file);
	errno = 0;
	assert(fread(buffer, 1, sizeof(buffer), file) == 0);
	assert(errno == EBADF);
	assert(ferror(file));
	assert(!feof(file));
	fclose(file);

	// The standard streams carry an access mode as well.
	errno = 0;
	assert(fwrite(str, 1, 1, stdin) == 0);
	assert(errno == EBADF);
	assert(ferror(stdin));
	clearerr(stdin);

	errno = 0;
	assert(fread(buffer, 1, 1, stdout) == 0);
	assert(errno == EBADF);
	assert(ferror(stdout));
	clearerr(stdout);

	// Check that stdout, stdin and stderr can be closed by the application (issue #12).
	fclose(stdout);
	fclose(stdin);
	fclose(stderr);

	return 0;
}
