
#include <pty.h>
#include <bits/ensure.h>

int openty(int *, int *, char *, const struct termios *, const struct winsize *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int forkpty(int *, char *, const struct termios *, const struct winsize *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

