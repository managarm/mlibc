#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

int main() {
	errno = 0;
	void *ptr = calloc((SIZE_MAX / 2) + 2, 2);
	assert(!ptr);
	assert(errno);

	errno = 0;
	ptr = calloc(10, sizeof(size_t));
	assert(ptr);
	for(size_t i = 0; i < 10; i++) {
		size_t *p = ptr;
		assert(!p[i]);
	}

	return 0;
}
