#include <bits/ensure.h>
#include <malloc.h>

void *memalign(size_t, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
