#include <bits/linux/linux_unistd.h>
#include <bits/ensure.h>

#include <unistd.h>

int dup3(int, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int vhangup(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getdtablesize(void){
	return sysconf(_SC_OPEN_MAX);
}
