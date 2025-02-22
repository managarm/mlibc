
#include <stdio_ext.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>

size_t __fbufsize(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

size_t __fpending(FILE *file_base) {
	__ensure(file_base->__dirty_end >= file_base->__dirty_begin);
	return file_base->__dirty_end - file_base->__dirty_begin;
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

int __freading(FILE *file_base) {
	return file_base->__io_mode == 0;
}

int __fwriting(FILE *file_base) {
	return file_base->__io_mode == 1;
}

int __fsetlocking(FILE *, int) {
	mlibc::infoLogger() << "mlibc: __fsetlocking() is a no-op" << frg::endlog;
	return FSETLOCKING_INTERNAL;
}

void _flushlbf(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// The following functions are defined by musl.

size_t __freadahead(FILE *file_base) {
	if(file_base->__io_mode != 0) {
		mlibc::infoLogger() << "mlibc: __freadahead() called but file is not open for reading" << frg::endlog;
		return 0;
	}
	return file_base->__valid_limit - file_base->__offset;
}

const char *__freadptr(FILE *, size_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void __freadptrinc(FILE *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void __fseterr(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

