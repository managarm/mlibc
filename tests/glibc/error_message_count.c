#include <assert.h>
#include <errno.h>
#include <error.h>

int main() {
	assert(error_message_count == 0);
	error(0, EINVAL, "test error #1");
	assert(error_message_count == 1);
	error_at_line(0, EINVAL, __FILE__, __LINE__, "test error #2");
	assert(error_message_count == 2);

	return 0;
}
