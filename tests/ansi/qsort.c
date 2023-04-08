#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const char *arr[] = { "xyz", "abc", "ghi", "def" };

static const size_t magic = 0xDEADBEEF;

static int cmpstringp(const void *p1, const void *p2, void *ctx) {
	/* The actual arguments to this function are "pointers to
	 * pointers to char", but strcmp(3) arguments are "pointers
	 * to char", hence the following cast plus dereference. */
	assert(*(size_t *) ctx == magic);
	return strcmp(*(const char **) p1, *(const char **) p2);
}

int main() {
	qsort_r(&arr[0], sizeof(arr) / sizeof(*arr), sizeof(char *), cmpstringp, (void *) &magic);

	assert(!strcmp(arr[0], "abc"));
	assert(!strcmp(arr[1], "def"));
	assert(!strcmp(arr[2], "ghi"));
	assert(!strcmp(arr[3], "xyz"));

	for(size_t i = 0; i < sizeof(arr) / sizeof(*arr); i++) {
		fprintf(stderr, "%s\n", arr[i]);
	}

	return 0;
}
