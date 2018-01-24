
#include <sys/utsname.h>
#include <bits/ensure.h>

int uname(struct utsname *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

