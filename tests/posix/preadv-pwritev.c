#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "preadv-pwritev-host-libc.tmp"
#elif defined(USE_CROSS_LIBC)
#define TEST_FILE "preadv-pwritev-cross-libc.tmp"
#else
#define TEST_FILE "preadv-pwritev.tmp"
#endif

int main() {
	unlink(TEST_FILE);

	int fd = open(TEST_FILE, O_RDWR | O_CREAT, 0644);
	assert(fd != -1);

	assert(write(fd, "0123456789", 10) == 10);

	char str0[] = "hello ";
	char str1[] = "world!";
	struct iovec wbufs[3] = {
		{ .iov_base = str0, .iov_len = 6 },
		{ .iov_base = NULL, .iov_len = 0 },
		{ .iov_base = str1, .iov_len = 6 },
	};

	// Writes past the current end of the file and leaves the file offset alone.
	assert(pwritev(fd, wbufs, 3, 4) == 12);
	assert(lseek(fd, 0, SEEK_CUR) == 10);

	char a[3], b[5];
	struct iovec rbufs[3] = {
		{ .iov_base = a, .iov_len = 3 },
		{ .iov_base = NULL, .iov_len = 0 },
		{ .iov_base = b, .iov_len = 5 },
	};
	assert(preadv(fd, rbufs, 3, 2) == 8);
	assert(!memcmp(a, "23h", 3));
	assert(!memcmp(b, "ello ", 5));
	assert(lseek(fd, 0, SEEK_CUR) == 10);

	// A short read at the end of the file fills the buffers in order.
	memset(a, 0, sizeof(a));
	memset(b, 0, sizeof(b));
	assert(preadv(fd, rbufs, 3, 12) == 4);
	assert(!memcmp(a, "rld", 3));
	assert(b[0] == '!' && b[1] == 0);

	assert(preadv(fd, rbufs, 3, 100) == 0);

	assert(!close(fd));
	unlink(TEST_FILE);
	return 0;
}
