#include <assert.h>
#include <search.h>
#include <stddef.h>

int compare_ints(const void *a, const void *b) {
	int int_a = *(const int *)a;
	int int_b = *(const int *)b;
	if (int_a < int_b)
		return -1;
	if (int_a > int_b)
		return 1;
	return 0;
}

int main(void) {
	int arr[10] = {10, 20, 30, 40, 50};
	size_t n = 5;

	int key1 = 30;
	int *res1 = lsearch(&key1, arr, &n, sizeof(int), compare_ints);
	assert(res1 != NULL);
	assert(*res1 == 30);
	assert(res1 == &arr[2]);
	assert(n == 5);

	int key2 = 60;
	int *res2 = lsearch(&key2, arr, &n, sizeof(int), compare_ints);
	assert(res2 != NULL);
	assert(*res2 == 60);
	assert(res2 == &arr[5]);
	assert(n == 6);
	assert(arr[5] == 60);

	int empty_arr[5];
	size_t empty_n = 0;

	int key3 = 42;
	int *res3 = lsearch(&key3, empty_arr, &empty_n, sizeof(int), compare_ints);
	assert(res3 != NULL);
	assert(*res3 == 42);
	assert(res3 == &empty_arr[0]);
	assert(empty_n == 1);
	assert(empty_arr[0] == 42);

	return 0;
}
