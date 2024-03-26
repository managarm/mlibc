#include <errno.h>
#include <error.h>

int main() {
	error(0, EINVAL, "test: %s", "error");

	return 0;
}
