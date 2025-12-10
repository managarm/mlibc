#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
	unsigned short xsubi[3] = { 42, 1337, 9001 };
	long value = jrand48(xsubi);
	assert(value >= INT32_MIN && value <= INT32_MAX);
	return 0;
}
