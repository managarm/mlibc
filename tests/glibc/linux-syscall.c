#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main() {
	char *data = "hello mlibc\n";
	long ret = syscall(SYS_write, 1, data, strlen(data));
	assert(ret == (long)strlen(data));

	syscall(SYS_exit, 0);
	abort();
}
