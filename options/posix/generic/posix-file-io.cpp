#include <mlibc/posix-file-io.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>

namespace mlibc {

mem_file::mem_file(char **ptr, size_t *sizeloc, void (*do_dispose)(abstract_file *))
: abstract_file{do_dispose}, _bufloc{ptr}, _sizeloc{sizeloc}, _buf{getAllocator()}, _pos{0} { }

int mem_file::close() {
	_update_ptrs();
	_buf.detach();
	return 0;
}

int mem_file::determine_type(stream_type *type) {
	*type = stream_type::file_like;
	return 0;
}

int mem_file::determine_bufmode(buffer_mode *mode) {
	*mode = buffer_mode::no_buffer;
	return 0;
}

int mem_file::io_read(char *, size_t, size_t *) {
	return EINVAL;
}

int mem_file::io_write(const char *buffer, size_t max_size, size_t *actual_size) {
	if (_pos + max_size >= _buf.size()) {
		_buf.resize(_pos + max_size + 1, '\0');
		_update_ptrs();
	}

	memcpy(_buf.data() + _pos, buffer, max_size);
	_pos += max_size;
	*actual_size = max_size;
	return 0;
}

int mem_file::io_seek(off_t offset, int whence, off_t *new_offset) {
	switch (whence) {
		case SEEK_SET:
			_pos = offset;
			if (_pos >= _buf.size()) {
				_buf.resize(_pos + 1, '\0');
				_update_ptrs();
			}
			*new_offset = _pos;
			break;
		case SEEK_CUR:
			_pos += offset;
			if (_pos >= _buf.size()) {
				_buf.resize(_pos + 1, '\0');
				_update_ptrs();
			}
			*new_offset = _pos;
			break;
		case SEEK_END:
			_pos = _buf.size() ? _buf.size() - 1 + offset : _buf.size() + offset;
			_buf.resize(_pos + 1, '\0');
			_update_ptrs();
			*new_offset = _pos;
			break;
		default:
			__ensure(!"Unknown whence value passed!");
			__builtin_unreachable();
	}
	return 0;
}

void mem_file::_update_ptrs() {
	*_bufloc = _buf.data();
	*_sizeloc = _buf.size() - 1;
}

buf_file::buf_file(char *ptr, size_t size, void (*do_dispose)(abstract_file *))
: abstract_file{do_dispose}, _buf{ptr}, _size{size}, _was_allocated{false}, _do_dispose(do_dispose) {
	if (!_buf) {
		_buf = reinterpret_cast<char *>(getAllocator().allocate(_size));
		_was_allocated = true;
	}
}

int buf_file::close() {
	if (_do_dispose)
		_do_dispose(this);
	if (_was_allocated)
		getAllocator().free(_buf);
	_buf = nullptr;
	return 0;
}

int buf_file::determine_type(stream_type *type) {
	*type = stream_type::file_like;
	return 0;
}

int buf_file::determine_bufmode(buffer_mode *mode) {
	*mode = buffer_mode::no_buffer;
	return 0;
}

int buf_file::io_read(char *buffer, size_t max_size, size_t *actual_size) {
	if (_pos >= _size) {
		*actual_size = 0;
		return 0;
	}
	if (max_size == 0) {
		*actual_size = 0;
		return 0;
	}
	size_t bytes_read = std::min(_size - _pos, max_size);
	memcpy(buffer, _buf + _pos, bytes_read);
	_pos += bytes_read;
	*actual_size = bytes_read;
	return 0;
}

int buf_file::io_write(const char *buffer, size_t max_size, size_t *actual_size) {
	if (_pos >= _size) {
		*actual_size = 0;
		return 0;
	}
	if (max_size == 0) {
		*actual_size = 0;
		return 0;
	}
	size_t bytes_written = std::min(_size - _pos, max_size);
	memcpy(_buf + _pos, buffer, bytes_written);
	_pos += bytes_written;
	*actual_size = bytes_written;
	return 0;
}

int buf_file::io_seek(off_t offset, int whence, off_t *new_offset) {
	switch(whence) {
		case SEEK_SET: {
			if(offset > _size)
				return EINVAL;
			_pos = offset;
			break;
		}
		case SEEK_CUR: {
			if(_pos + offset > _size)
				return EINVAL;
			_pos += offset;
			break;
		}
		case SEEK_END: {
			if(_size + offset > _size)
				return EINVAL;
			_pos = _size + offset;
			break;
		}
		default:
			return EINVAL;
	}
	*new_offset = _pos;
	return 0;
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
