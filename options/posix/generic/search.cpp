
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
		if (abs(height_a - height_b) < 2) {
			int old = node->h;
			node->h = height_a < height_b ? height_b + 1 : height_a + 1;
			return node->h - old;
		}

		return rotate(nodep, height_a < height_b);
	}
} // namespace

void *tsearch(const void *key, void **rootp, int(*compar)(const void *, const void *)) {
	if (!rootp)
		return nullptr;

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
		return nullptr;
	insert->key = key;
	insert->a[0] = insert->a[1] = nullptr;
	insert->h = 1;

	(*nodes.top()) = insert;
	nodes.pop();
	while(nodes.size() && balance_tree(nodes.top())) nodes.pop();
	return insert;
}

// This implementation is taken from musl
void *tfind(const void *key, void *const *rootp, int (*compar)(const void *, const void *)) {
	if(!*rootp)
		return nullptr;

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

void *tdelete(const void *key, void **rootp, int(*compar)(const void *, const void *)) {
	if (!*rootp)
		return nullptr;

	struct node *node_parent = nullptr;
	struct node *n = static_cast<struct node *>(*rootp);
	frg::stack<struct node **, MemoryAllocator> nodes(getAllocator());
	nodes.push(reinterpret_cast<struct node **>(rootp));
	int c = 0;
	for (;;) {
		if (!n)
			break;
		c = compar(key, n->key);
		if (!c)
			break;

		nodes.push(reinterpret_cast<struct node **>(&n->a[c > 0]));
		node_parent = n;
		n = static_cast<struct node *>(n->a[c > 0]);
	}

	if (!n)
		return nullptr;

	int child_count = 0;
	if (n->a[0])
		++child_count;
	if (n->a[1])
		++child_count;

	struct node **nodep = nodes.top();

	// 3 cases:
	// - no children, remove node
	// - one child, replace node with it
	// - two children, the predecessor replaces the node and balance up from the parent of the predecessor
	switch (child_count) {
		case 0:
			nodes.pop();
			*nodep = nullptr;
			break;
		case 1:
			*nodep = static_cast<struct node *>(n->a[0] ? n->a[0] : n->a[1]);
			break;
		case 2: {
			struct node *pred_parent = n;
			struct node *predecessor = static_cast<struct node *>(n->a[0]);

			while (predecessor->a[1]) {
				pred_parent = predecessor;
				predecessor = static_cast<struct node *>(predecessor->a[1]);
				nodes.push(reinterpret_cast<struct node **>(&pred_parent->a[1]));
			}

			// predecessor can only have a left child
			if (pred_parent == n) {
				// special case, predecessor is a direct child (left) of node
				// set right child to the deleted node's right child
				predecessor->a[1] = n->a[1];
			} else {
				// the predecessor will always be a right child
				// replace the predecessor's place with its left child (if any)
				pred_parent->a[1] = predecessor->a[0];
				predecessor->a[0] = static_cast<struct node *>(n->a[0]);
				predecessor->a[1] = static_cast<struct node *>(n->a[1]);
				nodes.pop();
			}
			*nodep = predecessor;
			break;
		}
	}

	free(static_cast<void *>(n));

	// go up balancing the tree
	while (!nodes.empty() && balance_tree(nodes.top()))
		nodes.pop();

	// NOTE: this WILL return a dangling pointer. this is expected behaviour (see glibc's manual page)
	return node_parent ? node_parent : n;
}

void twalk(const void *root, void (*action)(const void *, VISIT, int)) {
	if (!root)
		return;

	struct walk_node {
		const struct node *node;
		VISIT v;
	};
	int depth = 0;
	const struct node *node = static_cast<const struct node *>(root);
	frg::stack<struct walk_node, MemoryAllocator> nodes(getAllocator());

	struct walk_node wn = {node, VISIT::preorder};
	nodes.push(wn);

	while(!nodes.empty()) {
		wn = nodes.top();
		nodes.pop();

		if (!wn.node->a[0] && !wn.node->a[1])
			wn.v = VISIT::leaf;

		action(wn.node, wn.v, depth);

		struct node *next_child = nullptr;
		switch (wn.v) {
			case VISIT::endorder:
			case VISIT::leaf:
				--depth;
				break;
			case VISIT::preorder:
				next_child = static_cast<struct node *>(wn.node->a[0]);
				wn.v = VISIT::postorder;
				nodes.push(wn);
				break;
			case VISIT::postorder:
				next_child = static_cast<struct node *>(wn.node->a[1]);
				wn.v = VISIT::endorder;
				nodes.push(wn);
				break;
		}

		if (next_child) {
			++depth;
			struct walk_node tmp = {next_child, VISIT::preorder};
			nodes.push(tmp);
		}
	}
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
} // namespace

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
