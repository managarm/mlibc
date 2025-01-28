#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "access-host-libc.tmp"
#else
#define TEST_FILE "access.tmp"
#endif

int main() {
	// Make sure that it wasn't created by a previous test run
	unlink(TEST_FILE);

	assert(access(TEST_FILE, F_OK) == -1);
	assert(access(TEST_FILE, W_OK) == -1);
	assert(access(TEST_FILE, R_OK) == -1);
	assert(access(TEST_FILE, X_OK) == -1);

	close(open(TEST_FILE, O_CREAT | O_RDWR, 0666));

	assert(access(TEST_FILE, F_OK) == 0);
	assert(access(TEST_FILE, W_OK) == 0);
	assert(access(TEST_FILE, R_OK) == 0);
	assert(access(TEST_FILE, X_OK) == -1);

	unlink(TEST_FILE);

	return 0;
}
