#include <assert.h>

int foo(void);

int main(void) {
	int res = foo();
	assert(res == 420);

	return 0;
}
