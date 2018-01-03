
#include <libgen.h>
#include <mlibc/ensure.h>

char *basename(char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *dirname(char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

