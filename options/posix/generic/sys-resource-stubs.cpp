
#include <sys/resource.h>
#include <bits/ensure.h>

int getpriority(int, id_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getrlimit(int, struct rlimit *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getrusage(int, struct rusage *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setpriority(int, id_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setrlimit(int, const struct rlimit *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
