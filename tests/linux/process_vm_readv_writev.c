#include <assert.h>
#include <stdio.h>
#include <sys/uio.h>
#include <unistd.h>

// Test variable to read and write
static int test = 42;

int main() {
	// Read the variable using process_vm_readv
	int temp;
	struct iovec local_iov = {
		.iov_base = &temp,
		.iov_len = sizeof(temp),
	};
	struct iovec remote_iov = {
		.iov_base = &test,
		.iov_len = sizeof(test),
	};
	ssize_t bytes_read = process_vm_readv(getpid(), &local_iov, 1, &remote_iov, 1, 0);
	assert(bytes_read == sizeof(test));
	assert(temp == 42);

	// Write a new value to the variable using process_vm_writev
	int new_value = 1337;
	struct iovec new_local_iov = {
		.iov_base = &new_value,
		.iov_len = sizeof(new_value),
	};
	ssize_t bytes_written = process_vm_writev(getpid(), &new_local_iov, 1, &remote_iov, 1, 0);
	assert(bytes_written == sizeof(new_value));
	assert(test == 1337);

	return 0;
}
