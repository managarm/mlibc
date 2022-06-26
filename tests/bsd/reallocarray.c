#include <stdlib.h>

int main() {
	void *ret = reallocarray(NULL, 69, 0xCB7);
	free(ret);
	return !ret;
}
