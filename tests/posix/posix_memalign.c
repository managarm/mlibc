#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <errno.h>

int main() {
	void *p = NULL;

	// align must be a power of two
	assert(posix_memalign(&p, 3, 1) == EINVAL && p == NULL);

	// align must be a multiple of sizeof(void *)
	assert(posix_memalign(&p, sizeof(void *) / 2, 8) == EINVAL && p == NULL);

	assert(posix_memalign(&p, sizeof(void *), sizeof(void *)) == 0 && p != NULL && (uintptr_t)p % sizeof(void *) == 0);
	free(p);
	assert(posix_memalign(&p, 256, 1) == 0 && p != NULL && (uintptr_t)p % 256 == 0);
	free(p);
	assert(posix_memalign(&p, 256, 256) == 0 && p != NULL && (uintptr_t)p % 256 == 0);
	free(p);

	return 0;
}
