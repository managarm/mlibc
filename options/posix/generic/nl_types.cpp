#include <nl_types.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>

char *catgets(nl_catd, int, int, const char *) {
    __ensure(!"Not implemented");
	__builtin_unreachable();
}

