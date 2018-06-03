
#include <utime.h>

#include <bits/ensure.h>
#include <frigg/debug.hpp>

int utime(const char *, const struct utimbuf *times) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

