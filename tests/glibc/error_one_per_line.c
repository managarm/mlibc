#include <assert.h>
#include <errno.h>
#include <error.h>

int main() {
	assert(error_one_per_line == 0);
	assert(error_message_count == 0);
	error_one_per_line = 1;
	error_at_line(0, EINVAL, __FILE__, 0, "test error #1");
	assert(error_message_count == 1);
	error_at_line(0, EINVAL, __FILE__, 0, "test error #2");
	assert(error_message_count == 1);

	return 0;
}
