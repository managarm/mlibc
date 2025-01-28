#include <errno.h>
#include <error.h>

int main() {
	error_at_line(0, EINVAL, "error_at_line", 5, "test: %s", "error");

	return 0;
}
