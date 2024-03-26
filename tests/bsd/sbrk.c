#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	void *ret = sbrk(0);
	assert(ret != (void *) -1);
	assert(ret);

	return 0;
}
