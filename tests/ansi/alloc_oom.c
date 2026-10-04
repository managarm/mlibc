#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <errno.h>

int main() {
	void *p;

	// An allocation the system cannot possibly satisfy has to fail by returning
	// NULL, not by aborting the process.
	errno = 0;
	p = malloc(SIZE_MAX / 4);
	assert(p == NULL);
	assert(errno == ENOMEM);

	errno = 0;
	p = realloc(NULL, SIZE_MAX / 4);
	assert(p == NULL);
	assert(errno == ENOMEM);

	// The allocator has to stay usable afterwards.
	p = malloc(64);
	assert(p != NULL);
	free(p);

	return 0;
}
