
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#if __MLIBC_GLIBC_OPTION
#include <stdio_ext.h>
#endif

#include <bits/ensure.h>

#include <mlibc/debug.hpp>

#include <abi-bits/fcntl.h>
#include <frg/allocation.hpp>
#include <frg/mutex.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/file-io.hpp>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/lock.hpp>

namespace mlibc {

// --------------------------------------------------------------------------------------
// abstract_file implementation.
// --------------------------------------------------------------------------------------

namespace {
	using file_list = frg::intrusive_list<
		abstract_file,
		frg::locate_member<
			abstract_file,
			frg::default_list_hook<abstract_file>,
			&abstract_file::_list_hook
		>
	>;

	// Useful when debugging the FILE implementation.
	constexpr bool globallyDisableBuffering = false;

	// The maximum number of characters we permit the user to ungetc.
	constexpr size_t ungetBufferSize = 8;

	// List of files that will be flushed before exit().
	file_list &global_file_list() {
		static frg::eternal<file_list> list;
		return list.get();
	};
}

// For pipe-like streams (seek returns ESPIPE), we need to make sure
// that the buffer only ever contains all-dirty or all-clean data.
// Regarding _type and _bufmode:
//     As we might construct FILE objects for FDs that are not actually
//     open (e.g. for std{in,out,err}), we defer the type determination and cache the result.

abstract_file::abstract_file(void (*do_dispose)(abstract_file *))
: _type{stream_type::unknown}, _bufmode{buffer_mode::unknown}, _do_dispose{do_dispose} {
	// TODO: For __fwriting to work correctly, set the __io_mode to 1 if the write is write-only.
	__buffer_ptr = nullptr;
	__unget_ptr = nullptr;
	__buffer_size = 4096;
	__offset = 0;
	__io_offset = 0;
	__valid_limit = 0;
	__dirty_begin = 0;
	__dirty_end = 0;
	__io_mode = 0;
	__status_bits = 0;

	global_file_list().push_back(this);
}

abstract_file::~abstract_file() {
	if(__dirty_begin != __dirty_end)
		mlibc::infoLogger() << "mlibc warning: File is not flushed before destruction"
				<< frg::endlog;

	if(__buffer_ptr)
		getAllocator().free(__buffer_ptr - ungetBufferSize);

	auto it = global_file_list().iterator_to(this);
	global_file_list().erase(it);
}

void abstract_file::dispose() {
	if(!_do_dispose)
		return;
	_do_dispose(this);
}

// Note that read() and write() are asymmetric:
// While read() can trigger a write-back, write() can never trigger a read-ahead().
// This peculiarity is reflected in their code.

int abstract_file::read(char *buffer, size_t max_size, size_t *actual_size) {
	__ensure(max_size);

	if(_init_bufmode())
		return -1;

	size_t unget_length = 0;
	if (__unget_ptr != __buffer_ptr) {
		unget_length = frg::min(max_size, (size_t)(__buffer_ptr - __unget_ptr));
		memcpy(buffer, __unget_ptr, unget_length);

		__unget_ptr += unget_length;
		buffer += unget_length;
		max_size -= unget_length;

		if (max_size == 0) {
			*actual_size = unget_length;
			return 0;
		}
	}

	if(globallyDisableBuffering || _bufmode == buffer_mode::no_buffer) {
		size_t io_size;
		if(int e = io_read(buffer, max_size, &io_size); e) {
			__status_bits |= __MLIBC_ERROR_BIT;
			return e;
		}
		if(!io_size)
			__status_bits |= __MLIBC_EOF_BIT;
		*actual_size = io_size + unget_length;
		return 0;
	}

	// Ensure correct buffer type for pipe-like streams.
	// TODO: In order to support pipe-like streams we need to write-back the buffer.
	if(__io_mode && __valid_limit)
		mlibc::panicLogger() << "mlibc: Cannot read-write to same pipe-like stream"
				<< frg::endlog;
	__io_mode = 0;

	// Clear the buffer, then buffer new data.
	if(__offset == __valid_limit) {
		// TODO: We only have to write-back/reset if __valid_limit reaches the buffer end.
		if(int e = _write_back(); e)
			return e;
		if(int e = _reset(); e)
			return e;

		// Perform a read-ahead.
		_ensure_allocation();
		size_t io_size;
		if(int e = io_read(__buffer_ptr, __buffer_size, &io_size); e) {
			__status_bits |= __MLIBC_ERROR_BIT;
			return e;
		}
		if(!io_size) {
			__status_bits |= __MLIBC_EOF_BIT;
			*actual_size = 0;
			return 0;
		}

		__io_offset = io_size;
		__valid_limit = io_size;
	}

	// Return data from the buffer.
	__ensure(__offset < __valid_limit);

	auto chunk = frg::min(size_t(__valid_limit - __offset), max_size);
	memcpy(buffer, __buffer_ptr + __offset, chunk);
	__offset += chunk;

	*actual_size = chunk + unget_length;
	return 0;
}

int abstract_file::write(const char *buffer, size_t max_size, size_t *actual_size) {
	__ensure(max_size);

	if(_init_bufmode())
		return -1;
	if(globallyDisableBuffering || _bufmode == buffer_mode::no_buffer) {
		// As we do not buffer, nothing can be dirty.
		__ensure(__dirty_begin == __dirty_end);
		size_t io_size;
		if(int e = io_write(buffer, max_size, &io_size); e) {
			__status_bits |= __MLIBC_ERROR_BIT;
			return e;
		}
		*actual_size = io_size;
		return 0;
	}

	// Flush the buffer if necessary.
	if(__offset == __buffer_size) {
		if(int e = _write_back(); e)
			return e;
		if(int e = _reset(); e)
			return e;
	}

	// Ensure correct buffer type for pipe-like streams.
	// TODO: We could full support pipe-like files
	// by ungetc()ing all data before a write happens,
	// however, for now we just report an error.
	if(!__io_mode && __valid_limit) // TODO: Only check this for pipe-like streams.
		mlibc::panicLogger() << "mlibc: Cannot read-write to same pipe-like stream"
				<< frg::endlog;
	__io_mode = 1;

	__ensure(__offset < __buffer_size);
	auto chunk = frg::min(__buffer_size - __offset, max_size);

	// Line-buffered streams perform I/O on full lines.
	bool flush_line = false;
	if(_bufmode == buffer_mode::line_buffer) {
		auto nl = reinterpret_cast<char *>(memchr(buffer, '\n', chunk));
		if(nl) {
			chunk = nl + 1 - buffer;
			flush_line = true;
		}
	}
	__ensure(chunk);

	// Buffer data (without necessarily performing I/O).
	_ensure_allocation();
	memcpy(__buffer_ptr + __offset, buffer, chunk);

	if(__dirty_begin != __dirty_end) {
		__dirty_begin = frg::min(__dirty_begin, __offset);
		__dirty_end = frg::max(__dirty_end, __offset + chunk);
	}else{
		__dirty_begin = __offset;
		__dirty_end = __offset + chunk;
	}
	__valid_limit = frg::max(__offset + chunk, __valid_limit);
	__offset += chunk;

	// Flush line-buffered streams.
	if(flush_line) {
		if(_write_back())
			return -1;
	}

	*actual_size = chunk;
	return 0;
}

int abstract_file::unget(char c) {
	if (!__unget_ptr) {
		// This can happen if the file is unbuffered, but we still need
		// a space to store ungetc'd data.
		__ensure(!__buffer_ptr);
		_ensure_allocation();
		__ensure(__unget_ptr);
	}

	if ((size_t)(__buffer_ptr - __unget_ptr) + 1 > ungetBufferSize)
		return EOF;
	else {
		*(--__unget_ptr) = c;
		return c;
	}
}

int abstract_file::update_bufmode(buffer_mode mode) {
	// setvbuf() has undefined behavior if I/O has been performed.
	__ensure(__dirty_begin == __dirty_end
			&& "update_bufmode() must only be called before performing I/O");
	_bufmode = mode;
	return 0;
}

void abstract_file::purge() {
	__offset = 0;
	__io_offset = 0;
	__valid_limit = 0;
	__dirty_end = __dirty_begin;
	__unget_ptr = __buffer_ptr;
}

int abstract_file::flush() {
	if (__dirty_end != __dirty_begin) {
		if (int e = _write_back(); e)
			return e;
	}

	if (int e = _save_pos(); e)
		return e;
	purge();
	return 0;
}

int abstract_file::tell(off_t *current_offset) {
	off_t seek_offset;
	if(int e = io_seek(0, SEEK_CUR, &seek_offset); e)
		return e;

	*current_offset = seek_offset
		+ (off_t(__offset) - off_t(__io_offset))
		+ (off_t(__unget_ptr) - off_t(__buffer_ptr));
	return 0;
}

int abstract_file::seek(off_t offset, int whence) {
	if(int e = _write_back(); e)
		return e;

	off_t new_offset;
	if(whence == SEEK_CUR) {
		auto seek_offset = offset + (off_t(__offset) - off_t(__io_offset));
		if(int e = io_seek(seek_offset, whence, &new_offset); e) {
			__status_bits |= __MLIBC_ERROR_BIT;
			return e;
		}
	}else{
		__ensure(whence == SEEK_SET || whence == SEEK_END);
		if(int e = io_seek(offset, whence, &new_offset); e) {
			__status_bits |= __MLIBC_ERROR_BIT;
			return e;
		}
	}

	// We just forget the current buffer.
	// TODO: If the seek is "small", we can just modify our internal offset.
	purge();

	return 0;
}

int abstract_file::_init_type() {
	if(_type != stream_type::unknown)
		return 0;

	if(int e = determine_type(&_type); e)
		return e;
	__ensure(_type != stream_type::unknown);
	return 0;
}

int abstract_file::_init_bufmode() {
	if(_bufmode != buffer_mode::unknown)
		return 0;

	if(determine_bufmode(&_bufmode))
		return -1;
	__ensure(_bufmode != buffer_mode::unknown);
	return 0;
}

int abstract_file::_write_back() {
	if(int e = _init_type(); e)
		return e;

	if(__dirty_begin == __dirty_end)
		return 0;

	// For non-pipe streams, first do a seek to reset the
	// I/O position to zero, then do a write().
	if(_type == stream_type::file_like) {
		if(__io_offset != __dirty_begin) {
			__ensure(__dirty_begin - __io_offset > 0);
			off_t new_offset;
			if(int e = io_seek(off_t(__dirty_begin) - off_t(__io_offset), SEEK_CUR, &new_offset); e)
				return e;
			__io_offset = __dirty_begin;
		}
	}else{
		__ensure(_type == stream_type::pipe_like);
		__ensure(__io_offset == __dirty_begin);
	}

	// Now, we are in the correct position to write-back everything.
	while(__io_offset < __dirty_end) {
		size_t io_size;
		if(int e = io_write(__buffer_ptr + __io_offset, __dirty_end - __io_offset, &io_size); e) {
			__status_bits |= __MLIBC_ERROR_BIT;
			return e;
		}
		__ensure(io_size > 0 && "io_write() is expected to always write at least one byte");
		__io_offset += io_size;
		__dirty_begin += io_size;
	}

	return 0;
}

int abstract_file::_save_pos() {
	if (int e = _init_type(); e)
		return e;
	if (int e = _init_bufmode(); e)
		return e;

	if (_type == stream_type::file_like && _bufmode != buffer_mode::no_buffer) {
		off_t new_offset;
		auto seek_offset = (off_t(__offset) - off_t(__io_offset));
		if (int e = io_seek(seek_offset, SEEK_CUR, &new_offset); e) {
			__status_bits |= __MLIBC_ERROR_BIT;
			mlibc::infoLogger() << "hit io_seek() error " << e << frg::endlog;
			return e;
		}
		return 0;
	}
	return 0; // nothing to do for the rest
}

int abstract_file::_reset() {
	if(int e = _init_type(); e)
		return e;

	// For pipe-like files, we must not forget already read data.
	// TODO: Report this error to the user.
	if(_type == stream_type::pipe_like)
		__ensure(__offset == __valid_limit);

	__ensure(__dirty_begin == __dirty_end);
	__offset = 0;
	__io_offset = 0;
	__valid_limit = 0;

	return 0;
}

// This may still be called when buffering is disabled, for ungetc.
void abstract_file::_ensure_allocation() {
	if(__buffer_ptr)
		return;

	auto ptr = getAllocator().allocate(__buffer_size + ungetBufferSize);
	__buffer_ptr = reinterpret_cast<char *>(ptr) + ungetBufferSize;
	__unget_ptr = __buffer_ptr;
}

// --------------------------------------------------------------------------------------
// fd_file implementation.
// --------------------------------------------------------------------------------------

fd_file::fd_file(int fd, void (*do_dispose)(abstract_file *), bool force_unbuffered)
: abstract_file{do_dispose}, _fd{fd}, _force_unbuffered{force_unbuffered} { }

int fd_file::fd() {
	return _fd;
}

int fd_file::close() {
	if(__dirty_begin != __dirty_end)
		mlibc::infoLogger() << "mlibc warning: File is not flushed before closing"
				<< frg::endlog;
	if(int e = mlibc::sys_close(_fd); e)
		return e;
	return 0;
}

int fd_file::reopen(const char *path, const char *mode) {
	int mode_flags = parse_modestring(mode);

	int fd;
	if(int e = sys_open(path, mode_flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH, &fd); e) {
		return e;
	}

	flush();
	close();
	getAllocator().deallocate(__buffer_ptr, __buffer_size + ungetBufferSize);

	__buffer_ptr = nullptr;
	__unget_ptr = nullptr;
	__buffer_size = 4096;
	_reset();
	_fd = fd;

	if(mode_flags & O_APPEND) {
		seek(0, SEEK_END);
	}

	return 0;
}

int fd_file::determine_type(stream_type *type) {
	off_t offset;
	int e = mlibc::sys_seek(_fd, 0, SEEK_CUR, &offset);
	if(!e) {
		*type = stream_type::file_like;
		return 0;
	}else if(e == ESPIPE) {
		*type = stream_type::pipe_like;
		return 0;
	}else{
		return e;
	}
}

int fd_file::determine_bufmode(buffer_mode *mode) {
	// When isatty() is not implemented, we fall back to the safest default (no buffering).
	if(!mlibc::sys_isatty) {
		MLIBC_MISSING_SYSDEP();
		*mode = buffer_mode::no_buffer;
		return 0;
	}
	if(_force_unbuffered) {
		*mode = buffer_mode::no_buffer;
		return 0;
	}

	if(int e = mlibc::sys_isatty(_fd); !e) {
		*mode = buffer_mode::line_buffer;
		return 0;
	}else if(e == ENOTTY) {
		*mode = buffer_mode::full_buffer;
		return 0;
	}else{
		mlibc::infoLogger() << "mlibc: sys_isatty() failed while determining whether"
				" stream is interactive" << frg::endlog;
		return -1;
	}
}

int fd_file::io_read(char *buffer, size_t max_size, size_t *actual_size) {
	ssize_t s;
	if(int e = mlibc::sys_read(_fd, buffer, max_size, &s); e)
		return e;
	*actual_size = s;
	return 0;
}

int fd_file::io_write(const char *buffer, size_t max_size, size_t *actual_size) {
	ssize_t s;
	if(int e = mlibc::sys_write(_fd, buffer, max_size, &s); e)
		return e;
	*actual_size = s;
	return 0;
}

int fd_file::io_seek(off_t offset, int whence, off_t *new_offset) {
	if(int e = mlibc::sys_seek(_fd, offset, whence, new_offset); e)
		return e;
	return 0;
}

int fd_file::parse_modestring(const char *mode) {
	// Consume the first char; this must be 'r', 'w' or 'a'.
	int flags = 0;
	bool has_plus = strchr(mode, '+');
	if (*mode == 'r') {
		if (has_plus) {
			flags = O_RDWR;
		} else {
			flags = O_RDONLY;
		}
	} else if (*mode == 'w') {
		if (has_plus) {
			flags = O_RDWR;
		} else {
			flags = O_WRONLY;
		}
		flags |= O_CREAT | O_TRUNC;
	} else if (*mode == 'a') {
		if (has_plus) {
			flags = O_APPEND | O_RDWR;
		} else {
			flags = O_APPEND | O_WRONLY;
		}
		flags |= O_CREAT;
	} else {
		mlibc::infoLogger() << "Illegal fopen() mode '" << *mode << "'" << frg::endlog;
	}
	mode += 1;

	// Consume additional flags.
	while (*mode) {
		if (*mode == '+') {
			mode++; // This is already handled above.
		} else if (*mode == 'b') {
			mode++; // mlibc assumes that there is no distinction between text and binary.
		} else if (*mode == 'e') {
			flags |= O_CLOEXEC;
			mode++;
		} else if (*mode == 'x') {
			flags |= O_EXCL;
			mode++;
		} else {
			mlibc::infoLogger() << "Illegal fopen() flag '" << mode << "'" << frg::endlog;
			mode++;
		}
	}

	return flags;
}

} // namespace mlibc

namespace {
	mlibc::fd_file stdin_file{0};
	mlibc::fd_file stdout_file{1};
	mlibc::fd_file stderr_file{2, nullptr, true};

	struct stdio_guard {
		stdio_guard() { }

		~stdio_guard() {
			// Only flush the files but do not close them.
			for(auto it : mlibc::global_file_list()) {
				if(int e = it->flush(); e)
					mlibc::infoLogger() << "mlibc warning: Failed to flush file before exit()"
							<< frg::endlog;
			}
		}
	} global_stdio_guard;
}

FILE *stderr = &stderr_file;
FILE *stdin = &stdin_file;
FILE *stdout = &stdout_file;

int fileno_unlocked(FILE *file_base) {
	auto file = static_cast<mlibc::fd_file *>(file_base);
	return file->fd();
}

int fileno(FILE *file_base) {
	auto file = static_cast<mlibc::fd_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	return fileno_unlocked(file_base);
}

FILE *fopen(const char *path, const char *mode) {
	int flags = mlibc::fd_file::parse_modestring(mode);

	int fd;
	if(int e = mlibc::sys_open(path, flags, 0666, &fd); e) {
		errno = e;
		return nullptr;
	}

	return frg::construct<mlibc::fd_file>(getAllocator(), fd,
			mlibc::file_dispose_cb<mlibc::fd_file>);
}

int fclose(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	int e = 0;
	if(file->flush())
		e = EOF;
	if(file->close())
		e = EOF;
	file->dispose();
	return e;
}

int fseek(FILE *file_base, long offset, int whence) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	if(int e = file->seek(offset, whence); e) {
		errno = e;
		return -1;
	}
	return 0;
}

long ftell(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	off_t current_offset;
	if(int e = file->tell(&current_offset); e) {
		errno = e;
		return -1;
	}
	return current_offset;
}

int fflush_unlocked(FILE *file_base) {
	if(file_base == NULL) {
		// Only flush the files but do not close them.
		for(auto it : mlibc::global_file_list()) {
			if(int e = it->flush(); e)
				mlibc::infoLogger() << "mlibc warning: Failed to flush file"
					<< frg::endlog;
		}
		return 0;
	}
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	if(file->flush())
		return EOF;
	return 0;
}
int fflush(FILE *file_base) {
	if(file_base == NULL) {
		// Only flush the files but do not close them.
		for(auto it : mlibc::global_file_list()) {
			frg::unique_lock lock(it->_lock);
			if(int e = it->flush(); e)
				mlibc::infoLogger() << "mlibc warning: Failed to flush file"
					<< frg::endlog;
		}
		return 0;
	}

	auto file = static_cast<mlibc::abstract_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	if (file->flush())
		return EOF;
	return 0;
}

int setvbuf(FILE *file_base, char *, int mode, size_t) {
	// TODO: We could also honor the buffer, but for now use just set the mode.
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	if(mode == _IONBF) {
		if(int e = file->update_bufmode(mlibc::buffer_mode::no_buffer); e) {
			errno = e;
			return -1;
		}
	}else if(mode == _IOLBF) {
		if(int e = file->update_bufmode(mlibc::buffer_mode::line_buffer); e) {
			errno = e;
			return -1;
		}
	}else if(mode == _IOFBF) {
		if(int e = file->update_bufmode(mlibc::buffer_mode::full_buffer); e) {
			errno = e;
			return -1;
		}
	}else{
		errno = EINVAL;
		return -1;
	}

	return 0;
}

void rewind(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	file->seek(0, SEEK_SET);
	file_base->__status_bits &= ~(__MLIBC_EOF_BIT | __MLIBC_ERROR_BIT);
}

int ungetc(int c, FILE *file_base) {
	if (c == EOF)
		return EOF;

	auto file = static_cast<mlibc::abstract_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	return file->unget(c);
}

#if __MLIBC_GLIBC_OPTION
void __fpurge(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	frg::unique_lock lock(file->_lock);
	file->purge();
}
#endif

