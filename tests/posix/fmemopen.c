#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER0_SIZE 0x1000
#define BUFFER1 "Hello world"
#define BUFFER1_SIZE (sizeof(BUFFER1))

int main() {
	// test seek with mode "r"
	FILE *f = fmemopen(NULL, BUFFER0_SIZE, "r");
	assert(f);

	int ret = fseek(f, 0, SEEK_END);
	assert(!ret);
	long pos = ftell(f);
	fprintf(stderr, "pos = %ld\n", pos);
	// Despite the fact that this is correct behavior (see below),
	// this sometimes fails on glibc; newlib seems to get this wrong, too.
	// to quote what posix says about it:
	// > The stream shall also maintain the size of the current buffer contents;
	// > use of fseek() or fseeko() on the stream with SEEK_END shall seek relative to this size.
	// > For modes r and r+ the size shall be set to the value given by the size argument.
#if !defined(__GLIBC__)
	assert(pos == BUFFER0_SIZE);
#endif

	fclose(f);

	// test seek with mode "w"
	f = fmemopen(NULL, BUFFER0_SIZE, "w");
	assert(f);

	ret = fseek(f, 0, SEEK_END);
	assert(!ret);
	pos = ftell(f);
	assert(pos == 0);

	fclose(f);

	// test seek with mode "a" and a NULL buffer
	f = fmemopen(NULL, BUFFER0_SIZE, "a");
	assert(f);

	ret = fseek(f, 0, SEEK_END);
	assert(!ret);
	pos = ftell(f);
	assert(pos == 0);

	fclose(f);

	// test seek with mode "a" and a buffer containing a '\0'
	f = fmemopen(BUFFER1, BUFFER1_SIZE + 2, "a");
	assert(f);

	pos = ftell(f);
	assert(pos == (long) (BUFFER1_SIZE - 1));

	ret = fseek(f, 0, SEEK_SET);
	assert(!ret);
	pos = ftell(f);
	assert(!pos);

	ret = fseek(f, 0, SEEK_END);
	assert(!ret);
	pos = ftell(f);
	assert(pos == (long) (BUFFER1_SIZE - 1));

	fclose(f);

	// test seek with mode "a" and a buffer not containing a '\0'
	f = fmemopen(BUFFER1, BUFFER1_SIZE - 2, "a");
	assert(f);

	ret = fseek(f, 0, SEEK_END);
	assert(!ret);
	pos = ftell(f);
	assert(pos == (long) (BUFFER1_SIZE - 2));

	fclose(f);

	f = fmemopen(BUFFER1, BUFFER1_SIZE, "r");
	assert(f);

	ret = fseek(f, 0, SEEK_SET);
	assert(!ret);

	char buf[BUFFER1_SIZE];
	int read = fread(buf, 1, BUFFER1_SIZE - 2, f);
	assert(read == BUFFER1_SIZE - 2);
	assert(!strncmp(BUFFER1, buf, BUFFER1_SIZE - 2));

	fseek(f, 0, SEEK_END);

	read = fread(buf, 1, 2, f);
	assert(read == 0);
	assert(feof(f));

	fclose(f);

	// Open a buffer for read+write
	char *buf1 = strdup(BUFFER1);
	f = fmemopen(buf1, BUFFER1_SIZE, "w+");
	assert(f);
	assert(strlen(BUFFER1) == BUFFER1_SIZE - 1);

	// seek to somewhere in the middle of the buffer
	fseek(f, BUFFER1_SIZE - 5, SEEK_SET);
	// write as much data to it as possible
	read = fwrite(BUFFER1, 1, 9, f);
	rewind(f);

	// seek the the same position in the middle of the buffer
	ret = fseek(f, BUFFER1_SIZE - 5, SEEK_SET);
	assert(!ret);
	memset(buf, 0, BUFFER1_SIZE);
	// read what we just wrote
	read = fread(buf, 1, 5, f);
	// check that the write got correctly truncated
	fprintf(stderr, "buf '%s' (%zu)\n", buf, strlen(buf));
	assert(!strncmp(BUFFER1, buf, 4) && strlen(buf) == 4);

	fclose(f);
	free(buf1);

	char *buf2 = strdup(BUFFER1);
	f = fmemopen(buf2, 0, "r");
	assert(f || errno == EINVAL);

	if(f) {
		memset(buf, 0, BUFFER1_SIZE);
		read = fread(buf, 10, 1, f);
		assert(!read);
		rewind(f);

		read = fwrite(BUFFER1, 1, 12, f);
		assert(read == 0);

		fclose(f);
	}
	free(buf2);

	return 0;
}
