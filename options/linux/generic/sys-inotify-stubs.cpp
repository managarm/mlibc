
#include <sys/inotify.h>
#include <bits/ensure.h>

int inotify_init(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int inotify_init1(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int inotify_add_watch(int, const char *, unsigned int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int inotify_rm_watch(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

