#include <search.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>

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

static int freed = 0;

static void free_key(void *key) {
	freed++;
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

	tdestroy(root, free_key);
	assert(freed == 12);

	assert(hcreate(3));

	// Search for a non-existent entry
	ENTRY entry = {
		.key = (char *)"foo",
		.data = (void *)0x12345678
	};
	ENTRY *result = hsearch(entry, FIND);
	assert(!result);
	assert(errno == ESRCH);

	// Add a couple keys
	assert(hsearch(entry, ENTER));
	entry.key = (char *)"bar";
	entry.data = (void *)0x87654321;
	assert(hsearch(entry, ENTER));
	entry.key = (char *)"baz";
	entry.data = (void *)0x12344321;
	assert(hsearch(entry, ENTER));

	// Make sure that we can't add more keys
	entry.key = (char *)"not existing";
	assert(!hsearch(entry, ENTER));
	assert(errno == ENOMEM);

	// Check that the entries are in the hash table
	entry.key = (char *)"baz";
	result = hsearch(entry, FIND);
	assert(result);
	assert(result->data == (void *)0x12344321);
	entry.key = (char *)"foo";
	result = hsearch(entry, FIND);
	assert(result);
	assert(result->data == (void *)0x12345678);
	entry.key = (char *)"bar";
	result = hsearch(entry, FIND);
	assert(result);
	assert(result->data == (void *)0x87654321);

	hdestroy();

	return 0;
}
