#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "dprintf-host-libc.tmp"
#else
#define TEST_FILE "dprintf.tmp"
#endif

int main() {
	int fd = open(TEST_FILE, O_RDWR | O_CREAT, 0666);
	assert(fd > -1);

	int ret = dprintf(fd, "aaa");
	assert(ret == 3);

	// Check if we can read back the same things.
	// Also checks to see if the fd is still open,
	// which is issue #199.

	off_t seek = lseek(fd, 0, SEEK_SET);
	assert(seek == 0);

	char buf[3];
	ssize_t num_read = read(fd, buf, 3);

	assert(num_read == 3);
	assert(buf[0] == 'a'
		&& buf[1] == 'a'
		&& buf[2] == 'a');

	// All the tests pass, now we can unlink the file.
	ret = unlink(TEST_FILE);
	assert(ret == 0);
	return 0;
}
