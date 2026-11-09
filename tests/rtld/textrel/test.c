#include <assert.h>

extern int (*const textrel_ptr)(void);

int main() {
	assert(textrel_ptr() == 42);
	return 0;
}
