#include <bits/linux/linux_unistd.h>
#include <bits/ensure.h>

int dup3(int fd, int newfd, int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int vhangup(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
