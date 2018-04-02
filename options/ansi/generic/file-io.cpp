
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

static bool disableBuffering = false;

struct abstract_file : __mlibc_file_base {
public:
	abstract_file() {
		__buffer_ptr = nullptr;
		__buffer_size = 16;
		__offset = 0;
		__io_offset = 0;
		__valid_limit = 0;
		__is_dirty = 0;
	}

	abstract_file(const abstract_file &) = delete;

	abstract_file &operator= (const abstract_file &) = delete;

	~abstract_file() {
		frigg::panicLogger() << "mlibc: Fix abstract_file destructor" << frigg::endLog;
	}

	virtual int close() = 0;

	int read(char *buffer, size_t max_size, size_t *actual_size) {
		__ensure(max_size);

		if(disableBuffering)
			return io_read(buffer, max_size, actual_size);

		// Try use return data that is already inside buffers.
		if(__offset < __valid_limit) {
			auto chunk = frigg::min(size_t(__valid_limit - __offset), max_size);
			memcpy(buffer, __buffer_ptr + __offset, chunk);
			__offset += chunk;

			*actual_size = chunk;
			return 0;
		}

		_write_back();
		__offset = 0;
		__io_offset = 0;
		__valid_limit = 0;

		// Buffer some data.
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

		if(disableBuffering)
			return io_write(buffer, max_size, actual_size);

		// We might have to seek before writing.
		__ensure(__offset == __valid_limit);

		return io_write(buffer, max_size, actual_size);
	}

	int seek(off_t offset, int whence) {
		_write_back();
		
		if(whence == SEEK_SET || whence == SEEK_END) {
			if(io_seek(offset, whence))
				return -1;
			
			// For absolute seeks we can just forget the current buffer.
			__offset = 0;
			__io_offset = 0;
			__valid_limit = 0;
			return 0;
		}else{
			__ensure(whence == SEEK_CUR); // TODO: Handle errors.
			frigg::panicLogger() << "mlibc: Implement relative seek for FILE" << frigg::endLog;
		}
	}

protected:
	virtual int io_read(char *buffer, size_t max_size, size_t *actual_size) = 0;
	virtual int io_write(const char *buffer, size_t max_size, size_t *actual_size) = 0;
	virtual int io_seek(off_t offset, int whence) = 0;

private:
	void _write_back() {
		// TODO: Write back dirty bytes if necessary.
		// We need to write back all data in [0, __valid_limit].
		// First do a seek to reset the I/O position to zero, then do a write().
		// For pipe-like files (seek returns ESPIPE), we need to make sure
		// that the buffer only ever contains all-dirty or all-clean data!
		// We can achieve this by ungetc()ing all data before a write happens.
		__ensure(!__is_dirty);
	}

	void _ensure_allocation() {
		__ensure(__buffer_size);
		if(__buffer_ptr)
			return;

		auto ptr = getAllocator().allocate(__buffer_size);
		__buffer_ptr = reinterpret_cast<char *>(ptr);
	}
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

int __mlibc_exactRead(int fd, void *buffer, size_t length) {
	size_t progress = 0;
	while(progress < length) {
		ssize_t chunk;
		if(mlibc::sys_read(fd, (char *)buffer + progress, length, &chunk))
			assert(!"error in mlibc exactread");
		
		__ensure(chunk > 0);

		progress += chunk;
	}

	return 0;
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

int fflush(FILE *stream) {
/*
	if(stream->bufferBytes == 0)
		return 0;
	
	size_t written = 0;
	while(written < stream->bufferBytes) {
		ssize_t written_bytes;
		if(mlibc::sys_write(stream->fd, stream->__buffer_ptr + written,
				stream->bufferBytes - written, &written_bytes)) {
			stream->bufferBytes = 0;
			return EOF;
		}

		written += written_bytes;
		__ensure(written <= stream->bufferBytes);
	}

	stream->bufferBytes = 0;
*/
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

