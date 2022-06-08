#include <signal.h>
#include <bits/ensure.h>

int tgkill(int, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
