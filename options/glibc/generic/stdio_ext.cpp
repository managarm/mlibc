
#include <mlibc/file-io.hpp>
#include <stdio_ext.h>
#include <bits/ensure.h>
#include <mlibc/debug.hpp>

size_t __fbufsize(FILE *file_base) {
	// returns the size of the buffer currently used by the given stream
	// note that this _excludes_ the size of the area reserved for ungetc
	return file_base->__buffer_size;
}

size_t __fpending(FILE *file_base) {
	__ensure(file_base->__dirty_end >= file_base->__dirty_begin);
	return file_base->__dirty_end - file_base->__dirty_begin;
}

int __flbf(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	// return a nonzero value if the stream is line-buffered, zero otherwise
	return file->bufmode() == mlibc::buffer_mode::line_buffer;
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

int __fsetlocking(FILE *file_base, int state) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	bool oldstate = file->_lock.uselock;
	if (state != FSETLOCKING_QUERY) {
		if (state == FSETLOCKING_BYCALLER) {
			file->_lock.uselock = false;
		} else {
			file->_lock.uselock = true;
		}
	}
	if (oldstate) {
		return FSETLOCKING_INTERNAL;
	} else {
		return FSETLOCKING_BYCALLER;
	}
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

void __fseterr(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	// set the error indicator for the stream
	file->__status_bits |= __MLIBC_ERROR_BIT;
}

