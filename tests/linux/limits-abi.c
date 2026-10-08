#include <assert.h>
#include <limits.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	(void) argc;
	(void) argv;

    assert(TTY_NAME_MAX == 32);
    assert(sysconf(_SC_TTY_NAME_MAX) == TTY_NAME_MAX);

    return 0;
}