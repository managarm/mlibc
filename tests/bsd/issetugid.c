#include <assert.h>
#include <unistd.h>

int main(void) {
	// The tests do not run through a set-user-ID or set-group-ID binary, so the
	// process never gained privileges it did not start with. That holds whether
	// or not the user running them is root.
	assert(!issetugid());
	return 0;
}
