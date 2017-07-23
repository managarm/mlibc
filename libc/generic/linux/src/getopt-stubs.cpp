
#include <getopt.h>
#include <mlibc/ensure.h>

int getopt_long(int, char *const[], const char *, const struct option *, int *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int getopt_long_only(int, char *const[], const char *, const struct option *, int *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

