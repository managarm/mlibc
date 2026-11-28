
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

int __freadable(FILE *file_base) {
	return static_cast<mlibc::abstract_file *>(file_base)->readable();
}

int __fwritable(FILE *file_base) {
	return static_cast<mlibc::abstract_file *>(file_base)->writable();
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

const char *__freadptr(FILE *file_base, size_t *size) {
	if(file_base->__io_mode != 0)
		return nullptr;

	// Bytes pushed back by ungetc() are consumed before the buffer contents,
	// and they are not necessarily contiguous with the buffered data.
	if(file_base->__unget_ptr != file_base->__buffer_ptr) {
		*size = file_base->__buffer_ptr - file_base->__unget_ptr;
		return file_base->__unget_ptr;
	}

	if(file_base->__offset == file_base->__valid_limit)
		return nullptr;

	*size = file_base->__valid_limit - file_base->__offset;
	return file_base->__buffer_ptr + file_base->__offset;
}

void __freadptrinc(FILE *file_base, size_t increment) {
	if(file_base->__unget_ptr != file_base->__buffer_ptr) {
		auto unget_length = size_t(file_base->__buffer_ptr - file_base->__unget_ptr);
		auto chunk = frg::min(unget_length, increment);
		file_base->__unget_ptr += chunk;
		increment -= chunk;
	}

	__ensure(file_base->__offset + increment <= file_base->__valid_limit);
	file_base->__offset += increment;
}

void __fseterr(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	// set the error indicator for the stream
	file->__status_bits |= __MLIBC_ERROR_BIT;
}

