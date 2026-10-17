#include <assert.h>
#include <limits.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	assert(TTY_NAME_MAX >= _POSIX_TTY_NAME_MAX);

	return 0;
}
