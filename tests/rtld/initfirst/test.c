#include <assert.h>
#include <string.h>

const char *get_order(void);
void foo(void);

int main() {
	foo();

	// libfoo depends on libbar, but is linked with -z initfirst, which sets DF_1_INITFIRST.
	// Hence, libfoo is initialized before libbar.
	assert(!strcmp(get_order(), "fb"));
	return 0;
}
