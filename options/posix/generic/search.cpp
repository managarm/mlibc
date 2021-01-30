
#include <bits/ensure.h>
#include <search.h>

struct node {
	const void *key;
	void *a[2];
	int h;
};

void *tsearch(const void *, void **, int(*compar)(const void *, const void *)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// This implementation is taken from musl
void *tfind(const void *key, void *const *rootp, int (*compar)(const void *, const void *)) {
	if(!rootp)
		return 0;

	struct node *n = (struct node *)*rootp;
	for(;;) {
		if(!n) 
			break;
		int c = compar(key, n->key);
		if(!c)
			break;
		n = (struct node *)n->a[c > 0];
	}
	return n;
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
