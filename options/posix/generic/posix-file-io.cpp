#include <mlibc/posix-file-io.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <errno.h>

namespace mlibc {

int mem_file::reopen(const char *, const char *) {
	mlibc::panicLogger() << "mlibc: freopen() on a mem_file stream is unimplemented!" << frg::endlog;
	return -1;
}

int mem_file::determine_type(stream_type *type) {
	*type = stream_type::file_like;
	return 0;
}

int mem_file::determine_bufmode(buffer_mode *mode) {
	*mode = buffer_mode::no_buffer;
	return 0;
}

memstream_mem_file::memstream_mem_file(char **ptr, size_t *sizeloc, int flags, void (*do_dispose)(abstract_file *))
: mem_file{flags, do_dispose}, _bufloc{ptr}, _sizeloc{sizeloc} { }


int memstream_mem_file::close() {
	_update_ptrs();
	_buf.detach();

	return 0;
}

int memstream_mem_file::io_read(char *buffer, size_t max_size, size_t *actual_size) {
	if ((_pos >= 0 && _pos >= _max_size) || !max_size) {
		*actual_size = 0;
		return 0;
	}

	size_t bytes_read = std::min(size_t(_max_size - _pos), max_size);
	memcpy(buffer, _buffer().data() + _pos, bytes_read);
	_pos += bytes_read;
	*actual_size = bytes_read;
	return 0;
}

int memstream_mem_file::io_write(const char *buffer, size_t max_size, size_t *actual_size) {
	if (_pos + max_size >= _buffer_size()) {
		_buf.resize(_pos + max_size + 1, '\0');
		_update_ptrs();
	}

	size_t bytes_write = std::min(static_cast<size_t>(_buffer_size() - _pos), max_size);
	memcpy(_buffer().data() + _pos, buffer, bytes_write);
	_pos += max_size;
	*actual_size = max_size;

	return 0;
}

int memstream_mem_file::io_seek(off_t offset, int whence, off_t *new_offset) {
	switch (whence) {
		case SEEK_SET:
			_pos = offset;
			if (_pos >= 0 && size_t(_pos) >= _buffer_size()) {
				_buf.resize(_pos + 1, '\0');
				_update_ptrs();
			}
			*new_offset = _pos;
			break;
		case SEEK_CUR:
			_pos += offset;
			if (_pos >= 0 && size_t(_pos) >= _buffer_size()) {
				_buf.resize(_pos + 1, '\0');
				_update_ptrs();
			}
			*new_offset = _pos;
			break;
		case SEEK_END:
			_pos = _buffer_size() ? _buffer_size() - 1 + offset : _buffer_size() + offset;
			_buf.resize(_pos + 1, '\0');
			_update_ptrs();
			*new_offset = _pos;
			break;
		default:
			return EINVAL;
	}
	return 0;
}

void memstream_mem_file::_update_ptrs() {
	*_bufloc = _buf.data();
	*_sizeloc = _buf.size() - 1;
}

fmemopen_mem_file::fmemopen_mem_file(void *in_buf, size_t size, int flags, void (*do_dispose)(abstract_file *))
: mem_file{flags, do_dispose}, _inBuffer{in_buf}, _inBufferSize{size} {
	if(!_inBuffer) {
		_inBuffer = getAllocator().allocate(size);
		_needsDeallocation = true;
	}

	if(_flags & O_APPEND) {
		// the initial seek-size for append is zero if buf was NULL, or the first '\0' found, or the size
		_max_size = (_needsDeallocation) ? 0 : strnlen(reinterpret_cast<char *>(_inBuffer), _inBufferSize);
		_pos = _max_size;
	} else if((_flags & O_WRONLY || _flags & O_RDWR) && _flags & O_CREAT && _flags & O_TRUNC) {
		// modes: "w", "w+"
		_max_size = 0;
	} else {
		_max_size = size;
	}
}

int fmemopen_mem_file::close() {
	if(_needsDeallocation) {
		getAllocator().free(_inBuffer);
	}

	return 0;
}

int fmemopen_mem_file::io_read(char *buffer, size_t max_size, size_t *actual_size) {
	if ((_pos >= 0 && _pos >= _max_size) || !max_size) {
		*actual_size = 0;
		return 0;
	}

	size_t bytes_read = std::min(size_t(_max_size - _pos), max_size);
	memcpy(buffer, _buffer().data() + _pos, bytes_read);
	_pos += bytes_read;
	*actual_size = bytes_read;
	return 0;
}

int fmemopen_mem_file::io_write(const char *buffer, size_t max_size, size_t *actual_size) {
	off_t bytes_write = std::min(static_cast<size_t>(_buffer_size() - _pos), max_size);
	memcpy(_buffer().data() + _pos, buffer, bytes_write);
	_pos += bytes_write;
	*actual_size = bytes_write;

	if(_pos > _max_size) {
		_max_size = _pos;
	}

	// upon flushing, we need to put a null byte at the current position or at the end of the buffer
	size_t null = _pos;
	// a special case is if the mode is set to updating ('+'), then it always goes at the end
	if(null >= _buffer_size() || _flags & O_RDWR) {
		null = _buffer_size() - 1;
	}

	if(_buffer_size()) {
		_buffer()[null] = '\0';
	}

	return 0;
}

int fmemopen_mem_file::io_seek(off_t offset, int whence, off_t *new_offset) {
	switch (whence) {
		case SEEK_SET:
			if(offset < 0 || size_t(offset) > _buffer_size()) {
				return EINVAL;
			}
			_pos = offset;
			*new_offset = _pos;
			break;
		case SEEK_CUR:
			// seeking to negative positions or positions larger than the buffer is disallowed in fmemopen(3)
			if((_pos + offset) < 0 || size_t(_pos + offset) > _buffer_size()) {
				return EINVAL;
			}
			_pos += offset;
			*new_offset = _pos;
			break;
		case SEEK_END:
			if((_max_size + offset) < 0 || size_t(_max_size + offset) > _buffer_size()) {
				return EINVAL;
			}
			_pos = _max_size + offset;
			*new_offset = _pos;
			break;
		default:
			return EINVAL;
	}
	return 0;
}

int cookie_file::close() {
	if(!_funcs.close) {
		return 0;
	}

	return _funcs.close(_cookie);
}

int cookie_file::reopen(const char *, const char *) {
	mlibc::panicLogger() << "mlibc: freopen() on a cookie_file stream is unimplemented!" << frg::endlog;
	return -1;
}

int cookie_file::determine_type(stream_type *type) {
	*type = stream_type::file_like;
	return 0;
}

int cookie_file::determine_bufmode(buffer_mode *mode) {
	*mode = buffer_mode::no_buffer;
	return 0;
}

int cookie_file::io_read(char *buffer, size_t max_size, size_t *actual_size) {
	if(!_funcs.read) {
		return EOF;
	}

	*actual_size = _funcs.read(_cookie, buffer, max_size);

	return 0;
}

int cookie_file::io_write(const char *buffer, size_t max_size, size_t *actual_size) {
	if(!_funcs.write) {
		return 0;
	}

	*actual_size = _funcs.write(_cookie, buffer, max_size);

	return 0;
}

int cookie_file::io_seek(off_t offset, int whence, off_t *new_offset) {
	if(!_funcs.seek) {
		return ENOTSUP;
	}

	*new_offset = offset;

	return _funcs.seek(_cookie, new_offset, whence);
}

} // namespace mlibc

FILE *fdopen(int fd, const char *mode) {
	int flags = mlibc::fd_file::parse_modestring(mode);

	flags &= ~O_TRUNC; // 'w' should not truncate the file

	if (flags & O_APPEND) {
		int cur_flags = fcntl(fd, F_GETFL, 0);
		if (cur_flags < 0) {
			errno = EINVAL;
			return nullptr;
		} else if (!(cur_flags & O_APPEND)) {
			if (fcntl(fd, F_SETFL, cur_flags | O_APPEND)) {
				errno = EINVAL;
				return nullptr;
			}
		}
	}

	if (flags & O_CLOEXEC) {
		if (fcntl(fd, F_SETFD, FD_CLOEXEC)) {
			errno = EINVAL;
			return nullptr;
		}
	}

	// TODO: We may need to activate line buffered mode for terminals.

	return frg::construct<mlibc::fd_file>(getAllocator(), fd,
			mlibc::file_dispose_cb<mlibc::fd_file>);
}
