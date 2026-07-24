#include <bits/ensure.h>
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>

void *memalign(size_t alignment, size_t size) {
	// Unlike posix_memalign(), memalign() permits alignments smaller than sizeof(void *).
	if (alignment < sizeof(void *))
		alignment = sizeof(void *);

	void *ptr = nullptr;
	int ret = posix_memalign(&ptr, alignment, size);
	if (ret) {
		errno = ret;
		return nullptr;
	}
	return ptr;
}
