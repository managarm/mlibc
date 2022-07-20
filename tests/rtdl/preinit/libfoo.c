#include <stdio.h>
#include <assert.h>

int fooDone = 0;

// DSOs do not support pre-initialization functions.

__attribute__((constructor))
void fooInit() {
	dprintf(1, "initialization function called in foo\n");

	assert(fooDone == 0);
	fooDone++;
}

int isFooDone() {
	return fooDone;
}
