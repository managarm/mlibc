#include <errno.h>
#include <error.h>

int main() {
	error(1, EINVAL, "test error #1");

	return 0;
}
