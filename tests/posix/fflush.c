#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef int (*flush_function)(FILE *);

static void test_flush(flush_function flush_all) {
	char filename[] = "fflush.XXXXXX";
	int fd = mkstemp(filename);
	assert(fd >= 0);

	FILE *failed = fdopen(fd, "w+");
	assert(failed);
	FILE *successful = tmpfile();
	assert(successful);

	// Initialize the file type before replacing the descriptor, so the
	// buffered write itself reaches io_write() and sets the error indicator.
	assert(fseek(failed, 0, SEEK_SET) == 0);

	const char failed_payload[] = "failed";
	const char successful_payload[] = "successful";
	assert(fwrite(failed_payload, 1, sizeof(failed_payload) - 1, failed) == sizeof(failed_payload) - 1);
	assert(fwrite(successful_payload, 1, sizeof(successful_payload) - 1, successful) == sizeof(successful_payload) - 1);

	int read_only_fd = open(filename, O_RDONLY);
	assert(read_only_fd >= 0);
	int failed_fd = fileno(failed);
	assert(failed_fd >= 0);
	int writable_fd = dup(failed_fd);
	assert(writable_fd >= 0);
	assert(dup2(read_only_fd, failed_fd) == failed_fd);
	assert(close(read_only_fd) == 0);
	assert(unlink(filename) == 0);

	assert(flush_all(NULL) == EOF);
	assert(ferror(failed));

	int successful_fd = fileno(successful);
	assert(successful_fd >= 0);
	assert(lseek(successful_fd, 0, SEEK_SET) == 0);
	char buffer[sizeof(successful_payload)] = {};
	assert(read(successful_fd, buffer, sizeof(successful_payload) - 1) == sizeof(successful_payload) - 1);
	assert(!memcmp(buffer, successful_payload, sizeof(successful_payload) - 1));

	assert(dup2(writable_fd, failed_fd) == failed_fd);
	assert(close(writable_fd) == 0);
	clearerr(failed);
	assert(fclose(successful) == 0);
	assert(fclose(failed) == 0);
}

int main(void) {
	test_flush(fflush);
	test_flush(fflush_unlocked);
	return 0;
}
