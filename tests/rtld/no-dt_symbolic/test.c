#include <assert.h>
#include <stdio.h>

int bar(void);

int foo(void) {
	return 2;
}

int main() {
	int x = bar();
	if (x != 1)
		fprintf(stderr, "x = %d\n", x);
	assert(x == 1);
	return 0;
}
