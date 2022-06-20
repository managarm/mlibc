#include <stdio.h>
#include <assert.h>
#include <string.h>

int main() {
	char buffer[10];
	int ret = snprintf(buffer, 10, "%d", 123456789);
	assert(strncmp("123456789", buffer, 10) == 0);
	assert(ret == 9);

	// We deliberately induce a warning here.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
	ret = snprintf(buffer, 10, "%d", 1234567890);
	assert(strncmp("123456789", buffer, 10) == 0);
	assert(ret == 10);
#pragma GCC diagnostic pop

	// mlibc issue #118.
	ret = snprintf(NULL, 0, "%d", 123456789);
	assert(ret == 9);
	return 0;
}
