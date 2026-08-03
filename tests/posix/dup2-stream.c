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
	int ret;
	ssize_t io_size;
	int file_fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	assert(file_fd >= 0);
	FILE *stream = fdopen(file_fd, "w");
	assert(stream);

	ret = fputs("file\n", stream);
	assert(ret >= 0);
	ret = fflush(stream);
	assert(ret == 0);

	int pipe_fds[2];
	ret = pipe(pipe_fds);
	assert(ret == 0);
	ret = dup2(pipe_fds[1], file_fd);
	assert(ret == file_fd);
	ret = close(pipe_fds[1]);
	assert(ret == 0);

	ret = fputs("pipe\n", stream);
	assert(ret >= 0);
	ret = fflush(stream);
	assert(ret == 0);
	assert(!ferror(stream));
	ret = fseek(stream, 0, SEEK_CUR);
	assert(ret == -1);
	assert(errno == ESPIPE);
	assert(!ferror(stream));
	ret = fseek(stream, 0, SEEK_SET);
	assert(ret == -1);
	assert(errno == ESPIPE);
	assert(!ferror(stream));

	char buffer[sizeof("pipe\n") - 1];
	io_size = read(pipe_fds[0], buffer, sizeof(buffer));
	assert(io_size == sizeof(buffer));
	assert(!memcmp(buffer, "pipe\n", sizeof(buffer)));

	ret = fclose(stream);
	assert(ret == 0);
	ret = close(pipe_fds[0]);
	assert(ret == 0);
	ret = unlink(TEST_FILE);
	assert(ret == 0);
	return 0;
}
