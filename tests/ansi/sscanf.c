#include <stdio.h>
#include <assert.h>

int main() {
	int x = 0;
	char buf[] = "12345";
	sscanf(buf, "%d", &x);
	assert(x == 12345);
	return 0;
}
