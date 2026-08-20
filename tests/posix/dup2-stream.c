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
	int ret, c;
	ssize_t io_size;
	off_t offset;
	int file_fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	assert(file_fd >= 0);
	io_size = write(file_fd, "ABC", 3); assert(io_size == 3);
	offset = lseek(file_fd, 0, SEEK_SET); assert(offset == 0);

	FILE *stream = fdopen(file_fd, "r");
	assert(stream);
	c = fgetc(stream); assert(c == 'A');

	int pipe_fds[2];
	ret = pipe(pipe_fds); assert(ret == 0);
	io_size = write(pipe_fds[1], "P", 1); assert(io_size == 1);
	ret = close(pipe_fds[1]); assert(ret == 0);
	ret = dup2(pipe_fds[0], file_fd); assert(ret == file_fd);
	ret = close(pipe_fds[0]); assert(ret == 0);

	ret = fflush(stream); assert(ret == 0);
	c = fgetc(stream); assert(c == 'B');
	c = fgetc(stream); assert(c == 'C');
	c = fgetc(stream); assert(c == 'P');

	ret = fclose(stream); assert(ret == 0);
	ret = unlink(TEST_FILE); assert(ret == 0);
}

static void test_update_stream_after_failed_seek(void) {
	int ret, c;
	ssize_t io_size;
	off_t offset;
	int file_fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	assert(file_fd >= 0);
	io_size = write(file_fd, "ABC", 3);
	assert(io_size == 3);
	offset = lseek(file_fd, 0, SEEK_SET);
	assert(offset == 0);

	FILE *stream = fdopen(file_fd, "r+");
	assert(stream);
	c = fgetc(stream);
	assert(c == 'A');

	int pipe_fds[2];
	ret = pipe(pipe_fds);
	assert(ret == 0);
	ret = dup2(pipe_fds[1], file_fd);
	assert(ret == file_fd);
	ret = close(pipe_fds[1]);
	assert(ret == 0);

	errno = 0;
	ret = fseek(stream, 0, SEEK_CUR);
	assert(ret == -1);
	assert(errno == ESPIPE);
	assert(!ferror(stream));

	errno = 0;
	ret = fputc('X', stream);
	assert(ret != EOF);
	ret = fflush(stream);
	assert(ret == EOF);
	assert(errno == ESPIPE);
	assert(!ferror(stream));

	ret = fclose(stream);
	assert(ret == EOF);
	ret = close(pipe_fds[0]);
	assert(ret == 0);
	ret = unlink(TEST_FILE);
	assert(ret == 0);
}

static void test_read_after_write(void) {
	int ret, c;
	ssize_t io_size;
	off_t offset;
	int file_fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	assert(file_fd >= 0);
	io_size = write(file_fd, "ABC", 3);
	assert(io_size == 3);
	offset = lseek(file_fd, 0, SEEK_SET);
	assert(offset == 0);

	FILE *stream = fdopen(file_fd, "r+");
	assert(stream);
	ret = fputc('X', stream);
	assert(ret != EOF);
	c = fgetc(stream);
	assert(c == 'B');

	ret = fclose(stream);
	assert(ret == 0);
	ret = unlink(TEST_FILE);
	assert(ret == 0);
}

static void test_write_after_read(void) {
	int ret, c;
	ssize_t io_size;
	off_t offset;
	int file_fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	assert(file_fd >= 0);
	io_size = write(file_fd, "ABC", 3); assert(io_size == 3);
	offset = lseek(file_fd, 0, SEEK_SET); assert(offset == 0);

	FILE *stream = fdopen(file_fd, "r+");
	assert(stream);
	c = fgetc(stream); assert(c == 'A');
	ret = fputc('X', stream); assert(ret != EOF);
	ret = fflush(stream); assert(ret == 0);
	ret = fclose(stream); assert(ret == 0);

	file_fd = open(TEST_FILE, O_RDONLY);
	assert(file_fd >= 0);
	char buffer[3];
	io_size = read(file_fd, buffer, sizeof(buffer)); assert(io_size == sizeof(buffer));
	assert(!memcmp(buffer, "AXC", sizeof(buffer)));
	ret = close(file_fd); assert(ret == 0);
	ret = unlink(TEST_FILE); assert(ret == 0);
}

static void test_replacement_back_to_regular_file(void) {
	int ret, c;
	ssize_t io_size;
	off_t offset;
	int file_fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	assert(file_fd >= 0);
	io_size = write(file_fd, "ABC", 3); assert(io_size == 3);
	offset = lseek(file_fd, 0, SEEK_SET); assert(offset == 0);

	FILE *stream = fdopen(file_fd, "r");
	assert(stream);
	c = fgetc(stream); assert(c == 'A');

	int pipe_fds[2];
	ret = pipe(pipe_fds); assert(ret == 0);
	ret = close(pipe_fds[1]); assert(ret == 0);
	ret = dup2(pipe_fds[0], file_fd); assert(ret == file_fd);
	ret = close(pipe_fds[0]); assert(ret == 0);

	errno = 0;
	ret = fseek(stream, 0, SEEK_CUR); assert(ret == -1);
	assert(errno == ESPIPE);
	c = fgetc(stream); assert(c == 'B');
	c = fgetc(stream); assert(c == 'C');
	c = fgetc(stream); assert(c == EOF);

	int replacement_fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0600);
	assert(replacement_fd >= 0);
	io_size = write(replacement_fd, "XY", 2); assert(io_size == 2);
	offset = lseek(replacement_fd, 0, SEEK_SET); assert(offset == 0);
	ret = dup2(replacement_fd, file_fd); assert(ret == file_fd);
	ret = close(replacement_fd); assert(ret == 0);
	clearerr(stream);

	c = fgetc(stream); assert(c == 'X');
	ret = fflush(stream); assert(ret == 0);
	c = fgetc(stream); assert(c == 'Y');

	ret = fclose(stream); assert(ret == 0);
	ret = unlink(TEST_FILE); assert(ret == 0);
}

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
	test_read_ahead_after_replacement();
	test_update_stream_after_failed_seek();
	test_read_after_write();
	test_write_after_read();
	test_replacement_back_to_regular_file();
	return 0;
}
