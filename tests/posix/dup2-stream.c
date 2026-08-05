#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "mlibc-dup2-stream-host-libc.tmp"
#elif defined(USE_CROSS_LIBC)
#define TEST_FILE "mlibc-dup2-stream-cross-libc.tmp"
#else
#define TEST_FILE "mlibc-dup2-stream.tmp"
#endif

int main(void) {
	int file_fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	assert(file_fd >= 0);
	FILE *stream = fdopen(file_fd, "w");
	assert(stream);

	assert(fputs("file\n", stream) >= 0);
	assert(fflush(stream) == 0);

	int pipe_fds[2];
	assert(pipe(pipe_fds) == 0);
	assert(dup2(pipe_fds[1], file_fd) == file_fd);
	assert(close(pipe_fds[1]) == 0);

	assert(fputs("pipe\n", stream) >= 0);
	assert(fflush(stream) == 0);
	assert(!ferror(stream));
	assert(fseek(stream, 0, SEEK_CUR) == -1);
	assert(errno == ESPIPE);
	assert(!ferror(stream));
	assert(fseek(stream, 0, SEEK_SET) == -1);
	assert(errno == ESPIPE);
	assert(!ferror(stream));

	char buffer[sizeof("pipe\n") - 1];
	assert(read(pipe_fds[0], buffer, sizeof(buffer)) == sizeof(buffer));
	assert(!memcmp(buffer, "pipe\n", sizeof(buffer)));

	assert(fclose(stream) == 0);
	assert(close(pipe_fds[0]) == 0);
	assert(unlink(TEST_FILE) == 0);
	return 0;
}
