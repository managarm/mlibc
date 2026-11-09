#include <assert.h>

int foo(void);
void interpose(void);

int main() {
	interpose();

	assert(foo() == 2);
	return 0;
}
