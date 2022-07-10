#define _GNU_SOURCE
#include <assert.h>
#include <string.h>

#define test_string(x, expectval) assert(strcmp(basename(x), expectval) == 0)

int main() {
	test_string("/usr/lib", "lib");
	test_string("/usr/", "");
	test_string("/", "");
	test_string(".", ".");
	test_string("..", "..");
}
