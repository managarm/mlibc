#include <assert.h>
#include <libgen.h>
#include <string.h>

int main() {
	/* intentionally a macro: basename modifies its argument */
#define test_string(x, expectval) do {                         \
		char str[] = x;                                \
		assert(strcmp(basename(str), expectval) == 0); \
	} while (0)

	test_string("/usr/lib", "lib");
	test_string("/usr/", "usr");
	test_string("/", "/");
	test_string(".", ".");
	test_string("..", "..");

	return 0;
}
