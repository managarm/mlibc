#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define TEST_FILE "fdopen.tmp"

int main() {
	int fd = open(TEST_FILE, O_CREAT | O_RDWR, 0666);
	assert(fd >= 0);

	char *str = "mlibc fdopen test";
	assert(write(fd, str, strlen(str)));

	// Seek to the beginning, then reopen with fdopen in append mode.
	lseek(fd, 0, SEEK_SET);
	FILE *file = fdopen(fd, "a");
	assert(file);

	// Append and close.
	str = " appended";
	fwrite(str, strlen(str), 1, file);
	fflush(file);
	fclose(file);

	// Open it again and check that the append succeeded.
	fd = open(TEST_FILE, O_RDONLY);
	assert(fd >= 0);
	file = fdopen(fd, "r");
	assert(file);
	str = "mlibc fdopen test appended";
	char buf[100] = {0};
	assert(fread(buf, 1, strlen(str), file));
	assert(!strcmp(buf, str));
	fclose(file);

	return 0;
}
