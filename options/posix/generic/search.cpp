
#include <bits/ensure.h>
#include <search.h>

void *tsearch(const void *, void **, int(*compar)(const void *, const void *)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *tfind(const void *, void *const *, int (*compar)(const void *, const void *)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *tdelete(const void *, void **, int(*compar)(const void *, const void *)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void twalk(const void *, void (*action)(const void *, VISIT, int)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void tdestroy(void *, void (*free_node)(void *)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
