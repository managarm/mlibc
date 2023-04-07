#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	void *p1 = malloc(1023);
	fprintf(stderr, "size: %zu\n", malloc_usable_size(p1));
	assert(malloc_usable_size(p1) >= 1023);
	free(p1);

	return 0;
}
