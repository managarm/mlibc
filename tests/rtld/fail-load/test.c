#include <assert.h>

int foo(void);

int main() {
	assert(foo() == 69);

	return 0;
}
