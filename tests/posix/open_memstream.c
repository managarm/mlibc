#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define WRITE_NO 1024

int main() {
	size_t size;
	char *buf;

	FILE *fp = open_memstream(&buf, &size);
	assert(fp);

	char c = 'A';
	for (size_t i = 0; i < WRITE_NO; i++)
		assert(fwrite(&c, sizeof(char), 1, fp) == 1);

	// Flush the file to update the pointers.
	assert(!fflush(fp));

	assert(size == WRITE_NO);
	for (size_t i = 0; i < size; i++)
		assert(buf[i] == c);

	// Check if the stream maintains a null-bye at the end.
	assert(buf[size] == '\0');

	// Stream should be expanded, size should be == 2*WRITE_NO.
	assert(!fseek(fp, WRITE_NO, SEEK_END));
	assert(!fflush(fp));
	assert(size == 2*WRITE_NO);
	assert(buf[size] == '\0');

	// Check if it's filled with zero's.
	for (size_t i = WRITE_NO; i < size; i++)
		assert(buf[i] == '\0');

	// Go back and overwrite the 0's with 'B'.
	assert(!fseek(fp, -WRITE_NO, SEEK_CUR));
	c = 'B';
	for (size_t i = 0; i < WRITE_NO; i++)
		assert(fwrite(&c, sizeof(char), 1, fp) == 1);

	// Check if that happened.
	assert(size == 2*WRITE_NO);
	for (size_t i = WRITE_NO; i < size; i++)
		assert(buf[i] == c);
	assert(buf[size] == '\0');

	// Go to the front and write 'B'.
	assert(!fseek(fp, 0, SEEK_SET));
	for (size_t i = 0; i < WRITE_NO; i++)
		assert(fwrite(&c, sizeof(char), 1, fp) == 1);

	// Check if that happened.
	assert(size == 2*WRITE_NO);
	for (size_t i = 0; i < size; i++)
		assert(buf[i] == c);
	assert(buf[size] == '\0');

	// Close the file, we have tested everything.
	assert(!fclose(fp));
	free(buf);
	return 0;
}
