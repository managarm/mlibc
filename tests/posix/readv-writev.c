#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "readv-writev-host-libc.tmp"
#else
#define TEST_FILE "readv-writev.tmp"
#endif

int main() {
	// Make sure that it wasn't created by a previous test run
	unlink(TEST_FILE);

	int fd = open(TEST_FILE, O_RDWR | O_CREAT, 0644);
	assert(fd != -1);

	char str0[] = "hello ";
	char str1[] = "world!";

	struct iovec bufs[2] = {
		{
			.iov_base = &str0,
			.iov_len = strlen(str0),
		},
		{
			.iov_base = &str1,
			.iov_len = strlen(str1),
		},
	};

	ssize_t written = writev(fd, bufs, 2);
	assert(written == 12);

	memset(&str0, 0, strlen(str0));
	memset(&str1, 0, strlen(str1));

	assert(!lseek(fd, 0, SEEK_SET));

	ssize_t read = readv(fd, bufs, 2);
	assert(read == 12);

	assert(!strncmp(str0, "hello ", 7));
	assert(!strncmp(str1, "world!", 7));

	assert(!close(fd));

	unlink(TEST_FILE);

	return 0;
}
