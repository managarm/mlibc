#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "getdelim-host-libc.tmp"
#else
#define TEST_FILE "getdelim.tmp"
#endif

int main(void) {
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	// We have to open the file for writing and then reading separately,
	// as mlibc doesn't allow us to do both at the same time (yet).
	fp = fopen(TEST_FILE, "w");
	assert(fp);
	fputs("foo\nbar\nbaz\nquux\n", fp);
	fclose(fp);

	fp = fopen(TEST_FILE, "r");
	assert(fp);
	while ((read = getline(&line, &len, fp)) != -1) {
		printf("read line of length %zd, capacity %zu\n", read, len);
	}
	assert(!ferror(fp));
	free(line);
	fclose(fp);

	size_t nchars = 10000;
	fp = fopen(TEST_FILE, "w");
	assert(fp);
	for (size_t i = 0; i < nchars; i++)
		fputc('a', fp);
	fputc('b', fp);
	fclose(fp);

	line = NULL;
	len = 0;
	fp = fopen(TEST_FILE, "r");
	assert(fp);
	while ((read = getdelim(&line, &len, 'b', fp)) != -1) {
		printf("read line of length %zd, capacity %zu\n", read, len);
		assert((size_t)read == nchars + 1);
	}

	assert(len > nchars + 1);
	assert(!ferror(fp));
	assert(feof(fp));

	assert(getdelim(&line, &len, 'b', fp) == -1);
	assert(feof(fp));

	free(line);
	fclose(fp);

	fp = fopen(TEST_FILE, "w");
	assert(fp);
	assert(fwrite("1234ef\0f", 1, 8, fp) == 8);
	fclose(fp);

	fp = fopen(TEST_FILE, "r");
	assert(fp);

	/* test with line = NULL and large len */
	line = NULL;
	len = (size_t) ~0;
	assert(getdelim(&line, &len, 'e', fp) == 5);
	assert(!memcmp(line, "1234e", 6));
	assert(5 < len);

	/* test handling of internal nulls */
	assert(getdelim(&line, &len, 'e', fp) == 3);
	assert(!memcmp(line, "f\0f", 4));
	assert(3 < len);

	/* test handling of EOF */
	assert(getdelim(&line, &len, 'e', fp) == -1);

	free(line);
	fclose(fp);

	// Delete the file
	unlink(TEST_FILE);

	return 0;
}
