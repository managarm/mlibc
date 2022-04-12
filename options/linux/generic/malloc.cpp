#include <bits/ensure.h>
#include <malloc.h>

size_t malloc_usable_size(void *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *memalign(size_t, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
