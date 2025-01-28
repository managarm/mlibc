#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <errno.h>

int main() {
	void *p;

	p = aligned_alloc(sizeof(void *), sizeof(void *));
	assert(p != NULL && (uintptr_t)p % sizeof(void *) == 0);
	free(p);

	p = aligned_alloc(256, 256);
	assert(p != NULL && (uintptr_t)p % 256 == 0);
	free(p);

	// small alignments are okay
	p = aligned_alloc(1, 8);
	assert(p != NULL);
	free(p);
	p = aligned_alloc(1, 1);
	assert(p != NULL);
	free(p);

	// It seems that glibc doesn't report error in these cases.
#if !(defined(USE_HOST_LIBC) && defined(__GLIBC__))
	// size % align must be 0
	p = aligned_alloc(256, 1);
	assert(errno == EINVAL);
	assert(p == NULL);

	// align must be a 'valid alignment supported by the implementation'
	p = aligned_alloc(3, 1);
	assert(errno == EINVAL);
	assert(p == NULL);
#endif

	return 0;
}
