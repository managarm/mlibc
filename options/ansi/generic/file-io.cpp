
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <bits/ensure.h>

#include <frigg/debug.hpp>

#include <bits/abi.h>
#include <mlibc/allocator.hpp>
#include <mlibc/sysdeps.hpp>

namespace mlibc {

// Useful when debugging the FILE implementation.
static bool globallyDisableBuffering = false;

enum class stream_type {
	unknown,
	file_like,
	pipe_like
};

// For pipe-like streams (seek returns ESPIPE), we need to make sure
// that the buffer only ever contains all-dirty or all-clean data.
struct abstract_file : __mlibc_file_base {
public:
	abstract_file()
	: _type{stream_type::unknown} {
		__buffer_ptr = nullptr;
		__buffer_size = 128;
		__offset = 0;
		__io_offset = 0;
		__valid_limit = 0;
		__dirty_begin = 0;
		__dirty_end = 0;
		__io_mode = 0;
	}

	abstract_file(const abstract_file &) = delete;

	abstract_file &operator= (const abstract_file &) = delete;

	~abstract_file() {
		frigg::panicLogger() << "mlibc: Fix abstract_file destructor" << frigg::endLog;
	}

	virtual int close() = 0;

	int read(char *buffer, size_t max_size, size_t *actual_size) {
		__ensure(max_size);

		if(globallyDisableBuffering)
			return io_read(buffer, max_size, actual_size);

		// Ensure correct buffer type for pipe-like streams.
		// TODO: In order to support pipe-like streams we need to write-back the buffer.
		if(__io_mode && __valid_limit)
			frigg::panicLogger() << "mlibc: Cannot read-write to same pipe-like stream"
					<< frigg::endLog;
		__io_mode = 0;

		// Try use return data that is already inside buffers.
		if(__offset < __valid_limit) {
			auto chunk = frigg::min(size_t(__valid_limit - __offset), max_size);
			memcpy(buffer, __buffer_ptr + __offset, chunk);
			__offset += chunk;

			*actual_size = chunk;
			return 0;
		}

		// Clear the buffer, then buffer new data.
		if(_write_back())
			return -1;
		if(_reset())
			return -1;

		_ensure_allocation();
		size_t io_size;
		if(io_read(__buffer_ptr, __buffer_size, &io_size))
			return -1;
		if(!io_size) {
			*actual_size = 0;
			return 0;
		}

		// Return some of the newly buffered data.
		auto chunk = frigg::min(io_size, max_size);
		memcpy(buffer, __buffer_ptr, chunk);
		__offset = chunk;
		__io_offset = io_size;
		__valid_limit = io_size;

		*actual_size = chunk;
		return 0;
	}

	int write(const char *buffer, size_t max_size, size_t *actual_size) {
		__ensure(max_size);

		if(globallyDisableBuffering)
			return io_write(buffer, max_size, actual_size);

		// Flush the buffer if necessary.
		if(__offset == __buffer_size) {
			if(_write_back())
				return -1;
			if(_reset())
				return -1;
		}

		// Ensure correct buffer type for pipe-like streams.
		// TODO: We could full support pipe-like files
		// by ungetc()ing all data before a write happens,
		// however, for now we just report an error.
		if(!__io_mode && __valid_limit) // TODO: Only check this for pipe-like streams.
			frigg::panicLogger() << "mlibc: Cannot read-write to same pipe-like stream"
					<< frigg::endLog;
		__io_mode = 1;

		// Buffer data without performing I/O.
		__ensure(__offset < __buffer_size);

		_ensure_allocation();
		auto chunk = frigg::min(__buffer_size - __offset, max_size);
		memcpy(__buffer_ptr + __offset, buffer, chunk);
		if(__dirty_begin != __dirty_end) {
			__dirty_begin = frigg::min(__dirty_begin, __offset);
			__dirty_end = frigg::max(__dirty_end, __offset + chunk);
		}else{
			__dirty_begin = __offset;
			__dirty_end = __offset + chunk;
		}
		__valid_limit = frigg::max(__offset + chunk, __valid_limit);
		__offset += chunk;

		*actual_size = chunk;
		return 0;
	}

	// TODO: For input files, discard the buffer.
	int flush() {
		if(_write_back())
			return -1;

		return 0;
	}

	int seek(off_t offset, int whence) {
		if(_write_back())
			return -1;

		if(whence == SEEK_SET || whence == SEEK_END) {
			// For absolute seeks we can just forget the current buffer.
			if(_reset())
				return -1;

			if(io_seek(offset, whence))
				return -1;
			return 0;
		}else{
			__ensure(whence == SEEK_CUR); // TODO: Handle errors.
			frigg::panicLogger() << "mlibc: Implement relative seek for FILE" << frigg::endLog;
			__builtin_unreachable();
		}
	}

protected:
	virtual int determine_type(stream_type *type) = 0;
	virtual int io_read(char *buffer, size_t max_size, size_t *actual_size) = 0;
	virtual int io_write(const char *buffer, size_t max_size, size_t *actual_size) = 0;
	virtual int io_seek(off_t offset, int whence) = 0;

private:
	int _update_type() {
		if(_type != stream_type::unknown)
			return 0;

		if(determine_type(&_type))
			return -1;
		__ensure(_type != stream_type::unknown);
		return 0;
	}

	int _write_back() {
		if(_update_type())
			return -1;

		if(__dirty_begin == __dirty_end)
			return 0;

		// For non-pipe streams, first do a seek to reset the
		// I/O position to zero, then do a write().
		if(_type == stream_type::file_like) {
			if(io_seek(off_t(__dirty_begin) - off_t(__io_offset), SEEK_CUR))
				return -1;
			__io_offset = __dirty_begin;
		}else{
			__ensure(_type == stream_type::pipe_like);
			__ensure(__io_offset == __dirty_begin);
		}

		// Now, we are in the correct position to write back everything.
		while(__io_offset < __dirty_end) {
			size_t chunk;
			if(io_write(__buffer_ptr + __io_offset, __dirty_end - __io_offset, &chunk))
				return -1;
			__io_offset += chunk;
			__dirty_begin += chunk;
		}

		return 0;
	}

	int _reset() {
		if(_update_type())
			return -1;

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

	void _ensure_allocation() {
		__ensure(__buffer_size);
		if(__buffer_ptr)
			return;

		auto ptr = getAllocator().allocate(__buffer_size);
		__buffer_ptr = reinterpret_cast<char *>(ptr);
	}

	// As we might construct FILE objects for FDs that are not actually
	// open (e.g. for std{in,out,err}), we defer the type determination and cache the result.
	stream_type _type;
};

struct fd_file : abstract_file {
	fd_file(int fd)
	: _fd{fd} { }

	int fd() {
		return _fd;
	}

	int close() override {
		if(mlibc::sys_close(_fd))
			return -1;
		return 0;
	}

protected:
// TODO: We should not modify errno on ESPIPE.
	int determine_type(stream_type *type) override {
		off_t offset;
		if(!mlibc::sys_seek(_fd, 0, SEEK_CUR, &offset)) {
			*type = stream_type::file_like;
			return 0;
		}

		if(errno == ESPIPE) {
			*type = stream_type::pipe_like;
			return 0;
		}else{
			return -1;
		}
	}

	int io_read(char *buffer, size_t max_size, size_t *actual_size) override {
		ssize_t s;
		if(mlibc::sys_read(_fd, buffer, max_size, &s))
			return -1;
		*actual_size = s;
		return 0;
	}

	int io_write(const char *buffer, size_t max_size, size_t *actual_size) override {
		ssize_t s;
		if(mlibc::sys_write(_fd, buffer, max_size, &s))
			return -1;
		*actual_size = s;
		return 0;
	}

	int io_seek(off_t offset, int whence) override {
		off_t new_offset;
		if(mlibc::sys_seek(_fd, offset, whence, &new_offset))
			return -1;
		return 0;
	}

private:
	// Underlying file descriptor.
	int _fd;
};

} // namespace mlibc

mlibc::fd_file stdinFile{0};
mlibc::fd_file stdoutFile{1};
mlibc::fd_file stderrFile{2};

void __mlibc_initStdio() {
}

FILE *stderr = &stderrFile;
FILE *stdin = &stdinFile;
FILE *stdout = &stdoutFile;

int fileno(FILE *file_base) {
	auto file = static_cast<mlibc::fd_file *>(file_base);
	return file->fd();
}

FILE *fopen(const char *__restrict filename, const char *__restrict mode) {
	int fd;
	if(!strcmp(mode, "r") || !strcmp(mode, "rb")) {
		if(mlibc::sys_open(filename, __MLIBC_O_RDONLY, &fd))
			return nullptr;
	}else if(!strcmp(mode, "re")) {
		if(mlibc::sys_open(filename, __MLIBC_O_RDONLY | __MLIBC_O_CLOEXEC, &fd))
			return nullptr;
	}else if(!strcmp(mode, "r+")) {
		if(mlibc::sys_open(filename, __MLIBC_O_RDWR, &fd))
			return nullptr;
	}else if(!strcmp(mode, "w")) {
		if(mlibc::sys_open(filename, __MLIBC_O_WRONLY | __MLIBC_O_CREAT, &fd))
			return nullptr;
	}else if(!strcmp(mode, "we")) {
		if(mlibc::sys_open(filename, __MLIBC_O_WRONLY | __MLIBC_O_CREAT
				| __MLIBC_O_CLOEXEC, &fd))
			return nullptr;
	}else{
		frigg::panicLogger() << "Illegal fopen() mode '" << mode << "'" << frigg::endLog;
	}

	return frigg::construct<mlibc::fd_file>(getAllocator(), fd);
}

FILE *fdopen(int fd, const char *mode) {
	frigg::infoLogger() << "\e[31mmlibc: fdopen() ignores the file mode"
			<< "\e[39m" << frigg::endLog;
	(void)mode;

	return frigg::construct<mlibc::fd_file>(getAllocator(), fd);
}

int fclose(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	if(file->close())
		return EOF;
	return 0;
}

size_t fread(void *__restrict buffer, size_t size, size_t count,
		FILE *__restrict file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	__ensure(size);
	__ensure(count);

	// Distinguish two cases here: If the object size is one, we perform byte-wise reads.
	// Otherwise, we try to read each object individually.
	if(size == 1) {
		size_t progress = 0;
		while(progress < count) {
			size_t chunk;
			if(file->read((char *)buffer + progress,
					count - progress, &chunk)) {
				// TODO: Handle I/O errors.
				frigg::infoLogger() << "mlibc: fread() I/O errors are not handled"
						<< frigg::endLog;
				break;
			}else if(!chunk) {
				// TODO: Handle eof.
				break;
			}

			progress += chunk;
		}
		
		return progress;
	}else{
		for(size_t i = 0; i < count; i++) {
			size_t progress = 0;
			while(progress < size) {
				size_t chunk;
				if(file->read((char *)buffer + i * size + progress,
						size - progress, &chunk)) {
					// TODO: Handle I/O errors.
					frigg::infoLogger() << "mlibc: fread() I/O errors are not handled"
							<< frigg::endLog;
					break;
				}else if(!chunk) {
					// TODO: Handle eof.
					break;
				}
				
				progress += chunk;
			}

			if(progress < size)
				return i;
		}

		return count;
	}
}

size_t fwrite(const void *__restrict buffer, size_t size, size_t count,
		FILE *__restrict file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	__ensure(size);
	__ensure(count);

	// Distinguish two cases here: If the object size is one, we perform byte-wise writes.
	// Otherwise, we try to write each object individually.
	if(size == 1) {
		size_t progress = 0;
		while(progress < count) {
			size_t chunk;
			if(file->write((const char *)buffer + progress,
					count - progress, &chunk)) {
				// TODO: Handle I/O errors.
				frigg::infoLogger() << "mlibc: fwrite() I/O errors are not handled"
						<< frigg::endLog;
				break;
			}else if(!chunk) {
				// TODO: Handle eof.
				break;
			}

			progress += chunk;
		}
		
		return progress;
	}else{
		for(size_t i = 0; i < count; i++) {
			size_t progress = 0;
			while(progress < size) {
				size_t chunk;
				if(file->write((const char *)buffer + i * size + progress,
						size - progress, &chunk)) {
					// TODO: Handle I/O errors.
					frigg::infoLogger() << "mlibc: fwrite() I/O errors are not handled"
							<< frigg::endLog;
					break;
				}else if(!chunk) {
					// TODO: Handle eof.
					break;
				}
				
				progress += chunk;
			}

			if(progress < size)
				return i;
		}

		return count;
	}
}

int fseek(FILE *stream, long offset, int whence) {
	frigg::panicLogger() << "mlibc: Fix fseek()" << frigg::endLog;
	/*
	off_t new_offset;
	if(mlibc::sys_seek(stream->fd, offset, whence, &new_offset))
		return -1;
	*/
	return 0;
}

long ftell(FILE *stream) {
	frigg::panicLogger() << "mlibc: Fix ftell()" << frigg::endLog;
	/*
	off_t new_offset;
	if(mlibc::sys_seek(stream->fd, 0, SEEK_CUR, &new_offset))
		return EOF;
	return new_offset;
	*/
	return 0;
}

int fflush(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	if(file->flush())
		return EOF;
	return 0;
}

int setvbuf(FILE *__restrict stream, char *__restrict buffer, int mode, size_t size) {
	frigg::panicLogger() << "mlibc: Fix setvbuf()" << frigg::endLog;
/*
	__ensure(mode == _IOLBF);
	__ensure(stream->bufferBytes == 0);

	// TODO: free the old buffer
	if(!buffer) {
		auto new_buffer = (char *)malloc(size); // TODO: Use the allocator.
		__ensure(new_buffer);
		stream->__buffer_ptr = new_buffer;
		stream->__buffer_size = size;
	}else{
		stream->__buffer_ptr = buffer;
		stream->__buffer_size = size;
	}
*/

	return 0;
}

void rewind(FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	file->seek(0, SEEK_SET);
	// TODO: rewind() should also clear the error indicator.
}

