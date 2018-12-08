
#include <stdio_ext.h>
#include <bits/ensure.h>

size_t __fbufsize(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
size_t __fpending(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int __flbf(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int __freadable(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int __fwritable(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int __freading(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int __fwriting(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int __fsetlocking(FILE *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void _flushlbf(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// The following functions are defined by musl.

size_t __freadahead(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
const char *__freadptr(FILE *, size_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void __fseterr(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

