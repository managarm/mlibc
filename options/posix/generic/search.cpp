
#include <bits/ensure.h>
#include <search.h>
#include <stddef.h>
#include <new>
#include <mlibc/allocator.hpp>
#include <mlibc/search.hpp>
#include <frg/stack.hpp>
#include <stdlib.h>

struct node {
	const void *key;
	void *a[2];
	int h;
};

namespace {
	int height(struct node *node) {
		return node ? node->h : 0;
	}

	int rotate(struct node **nodep, int side) {
		struct node *node = *nodep;
		struct node *x = static_cast<struct node *>(node->a[side]);
		struct node *y = static_cast<struct node *>(x->a[!side]);
		struct node *z = static_cast<struct node *>(x->a[side]);

		int height_node = node->h;
		int height_y = height(y);
		if (height_y > height(z)) {
			// Perform double rotation
			node->a[side] = y->a[!side];
			x->a[!side] = y->a[side];
			y->a[!side] = node;
			y->a[side] = x;
			node->h = height_y;
			x->h = height_y;
			y->h = height_y + 1;
		} else {
			// Perform single rotation
			node->a[side] = y;
			x->a[!side] = node;
			node->h = height_y + 1;
			x->h = height_y + 2;
			y = x;

		}
		*nodep = y;
		return y->h - height_node;
	}

	int balance_tree(struct node **nodep) {
		struct node *node = *nodep;
		int height_a = height(static_cast<struct node *>(node->a[0]));
		int height_b = height(static_cast<struct node *>(node->a[1]));
		if (height_a - height_b < 2) {
			int old = node->h;
			node->h = height_a < height_b ? height_b + 1 : height_a + 1;
			return node->h - old;
		}

		return rotate(nodep, height_a < height_b);
	}
}

void *tsearch(const void *key, void **rootp, int(*compar)(const void *, const void *)) {
	if (!rootp)
		return NULL;

	struct node *n = static_cast<struct node *>(*rootp);
	frg::stack<struct node **, MemoryAllocator> nodes(getAllocator());
	nodes.push(reinterpret_cast<struct node **>(rootp));
	int c = 0;
	for (;;) {
		if (!n)
			break;
		c = compar(key, n->key);
		if (!c)
			return n;
		nodes.push(reinterpret_cast<struct node **>(&n->a[c > 0]));
		n = static_cast<struct node *>(n->a[c > 0]);
	}

	struct node *insert = static_cast<struct node*>(malloc(sizeof(struct node)));
	if (!insert)
		return NULL;
	insert->key = key;
	insert->a[0] = insert->a[1] = NULL;
	insert->h = 1;

	(*nodes.top()) = insert;
	nodes.pop();
	while(nodes.size() && balance_tree(nodes.top())) nodes.pop();
	return insert;
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
	(void)compar;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void twalk(const void *, void (*action)(const void *, VISIT, int)) {
	(void)action;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void tdestroy(void *root, void (*free_node)(void *)) {
	auto *n = static_cast<node *>(root);
	frg::stack<node *, MemoryAllocator> nodes(getAllocator());

	while(n || !nodes.empty()) {
		if(n == nullptr) {
			n = nodes.top();
			nodes.pop();
			free_node(const_cast<void *>(n->key));
			auto *next = static_cast<node *>(n->a[1]);
			free(n);
			n = next;
		} else {
			nodes.push(n);
			n = static_cast<node *>(n->a[0]);
		}
	}
}

void *lsearch(const void *key, void *base, size_t *nelp, size_t width,
		int (*compar)(const void *, const void *)) {
	(void)key;
	(void)base;
	(void)nelp;
	(void)width;
	(void)compar;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *lfind(const void *key, const void *base, size_t *nelp,
		size_t width, int (*compar)(const void *, const void *)) {
	(void)key;
	(void)base;
	(void)nelp;
	(void)width;
	(void)compar;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

namespace {
	hsearch_data globalTable {};
}

int hcreate(size_t num_entries) {
	return mlibc::hcreate_r(num_entries, &globalTable);
}

void hdestroy(void) {
	mlibc::hdestroy_r(&globalTable);
}

ENTRY *hsearch(ENTRY item, ACTION action) {
	ENTRY *ret;
	if(mlibc::hsearch_r(item, action, &ret, &globalTable) == 0) {
		return nullptr;
	}
	return ret;
}
