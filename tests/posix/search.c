#include <search.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

static int compare(const void *pa, const void *pb) {
	if (*(int*)pa < *(int*) pb)
		return -1;
	if (*(int*)pa > *(int*) pb)
		return 1;
	return 0;
}

static void check_key(int key, void *root) {
	int keyp = key;
	void *ret = tfind((void*) &keyp, &root, compare);
	assert(ret);
	assert(**((int **) ret) == key);
}

static void free_key(void *key) {
	free(key);
}

int main() {
	void *root = NULL;
	for (int i = 0; i < 12; i++) {
		int *ptr = malloc(sizeof(int));
		assert(ptr);
		*ptr = i;

		void *ret = tsearch((void*) ptr, &root, compare);
		assert(ret);
		assert(**((int **) ret) == i);
	}

	// Test a couple of keys
	check_key(1, root);
	check_key(5, root);
	check_key(10, root);

	// Verify NULL on non-existent key
	int key = -1;
	void *ret = tfind((void*) &key, &root, compare);
	assert(ret == NULL);

	// tdelete is not implemented yet (#351)
	(void)free_key;
	// tdestroy(root, free_key);
	return 0;
}
