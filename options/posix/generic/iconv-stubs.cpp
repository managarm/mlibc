#include <iconv.h>
#include <bits/ensure.h>

size_t iconv(iconv_t, char **__restrict, size_t *__restrict, char **__restrict, size_t *__restrict) {
	__ensure(!"iconv() not implemented");
	__builtin_unreachable();
}

int iconv_close(iconv_t) {
	__ensure(!"iconv_close() not implemented");
	__builtin_unreachable();
}

iconv_t iconv_open(const char *, const char *) {
	__ensure(!"iconv_open() not implemented");
	__builtin_unreachable();
}


