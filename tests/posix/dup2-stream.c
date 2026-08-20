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

static void test_read_ahead_after_replacement(void) {
	int file_fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	assert(file_fd >= 0);
	assert(write(file_fd, "ABC", 3) == 3);
	assert(lseek(file_fd, 0, SEEK_SET) == 0);

	FILE *stream = fdopen(file_fd, "r");
	assert(stream);
	assert(fgetc(stream) == 'A');

	int pipe_fds[2];
	assert(pipe(pipe_fds) == 0);
	assert(write(pipe_fds[1], "P", 1) == 1);
	assert(close(pipe_fds[1]) == 0);
	assert(dup2(pipe_fds[0], file_fd) == file_fd);
	assert(close(pipe_fds[0]) == 0);

	assert(fflush(stream) == 0);
	assert(fgetc(stream) == 'B');
	assert(fgetc(stream) == 'C');
	assert(fgetc(stream) == 'P');

	assert(fclose(stream) == 0);
	assert(unlink(TEST_FILE) == 0);
}

static void test_update_stream_after_failed_seek(void) {
	int file_fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	assert(file_fd >= 0);
	assert(write(file_fd, "ABC", 3) == 3);
	assert(lseek(file_fd, 0, SEEK_SET) == 0);

	FILE *stream = fdopen(file_fd, "r+");
	assert(stream);
	assert(fgetc(stream) == 'A');

	int pipe_fds[2];
	assert(pipe(pipe_fds) == 0);
	assert(dup2(pipe_fds[1], file_fd) == file_fd);
	assert(close(pipe_fds[1]) == 0);

	errno = 0;
	assert(fseek(stream, 0, SEEK_CUR) == -1);
	assert(errno == ESPIPE);
	assert(!ferror(stream));

	errno = 0;
	assert(fputc('X', stream) != EOF);
	assert(fflush(stream) == EOF);
	assert(errno == ESPIPE);
	assert(!ferror(stream));

	assert(fclose(stream) == EOF);
	assert(close(pipe_fds[0]) == 0);
	assert(unlink(TEST_FILE) == 0);
}

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

	test_read_ahead_after_replacement();
	test_update_stream_after_failed_seek();
	return 0;
}
