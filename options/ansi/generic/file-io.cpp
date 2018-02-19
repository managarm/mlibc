
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <bits/ensure.h>

#include <frigg/debug.hpp>

#include <bits/abi.h>
#include <mlibc/sysdeps.hpp>

__mlibc_File stdinFile;
__mlibc_File stdoutFile;
__mlibc_File stderrFile;

void __mlibc_initStdio() {
	size_t buffer_size = 1024;
	
	stdinFile.fd = 0;
	stdinFile.bufferPtr = (char *)malloc(buffer_size);
	stdinFile.bufferSize = buffer_size;

	stdoutFile.fd = 1;
	stdoutFile.bufferPtr = (char *)malloc(buffer_size);
	stdoutFile.bufferSize = buffer_size;
	
	stderrFile.fd = 2;
	stderrFile.bufferPtr = (char *)malloc(buffer_size);
	stderrFile.bufferSize = buffer_size;
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

static bool disableBuffering = false;

FILE *fopen(const char *__restrict filename, const char *__restrict mode) {
	int fd;
	if(!strcmp(mode, "r") || !strcmp(mode, "rb")) {
		if(mlibc::sys_open(filename, __MLIBC_O_RDONLY, &fd))
			return nullptr;
	}else if(!strcmp(mode, "re")) {
		if(mlibc::sys_open(filename, __MLIBC_O_RDONLY | __MLIBC_O_CLOEXEC, &fd))
			return nullptr;
	}else{
		frigg::panicLogger() << "Illegal fopen() mode '" << mode << "'" << frigg::endLog;
	}

	FILE *file = (FILE *)malloc(sizeof(FILE));
	file->fd = fd;
	file->bufferPtr = nullptr;
	file->bufferSize = 0;
	return file;
}

size_t fread(void *__restrict buffer, size_t size, size_t count, FILE *__restrict stream) {
	// Distinguish two cases here: If the object size is one, we perform byte-wise reads.
	// Otherwise, we try to read each object individually.
	if(size == 1) {
		size_t progress = 0;
		while(progress < count) {
			ssize_t chunk;
			if(mlibc::sys_read(stream->fd, (char *)buffer + progress, count - progress, &chunk)) {
				// TODO: Handle I/O errors.
				return progress;
			}
			if(!chunk) {
				// TODO: Handle eof.
				return progress;
			}

			progress += chunk;
		}

		return count;
	}else{
		// TODO: Read each object individually.
		if(__mlibc_exactRead(stream->fd, buffer, size * count))
			return 0;
		return size * count;
	}
}

size_t fwrite(const void *__restrict buffer, size_t size, size_t count,
		FILE *__restrict stream) {
	for(size_t i = 0; i < count; i++) {
		char *block_ptr = (char *)buffer + i * size;
		if(size > stream->bufferSize || disableBuffering) {
			// write the buffer directly to the fd
			size_t written = 0;
			while(written < size) {
				ssize_t result;
				if(mlibc::sys_write(stream->fd, block_ptr + written, size - written, &result))
					return i;
				written += result;
				__ensure(written <= size);
			}
		}else if(stream->bufferBytes + size < stream->bufferSize) {
			memcpy((char *)stream->bufferPtr + stream->bufferBytes, block_ptr, size);
			stream->bufferBytes += size;
		}else{
			__ensure(!"Not implemented");
			__builtin_unreachable();
		}
	}

	return count;
}

int fseek(FILE *stream, long offset, int whence) {
	off_t new_offset;
	if(mlibc::sys_seek(stream->fd, offset, whence, &new_offset))
		return -1;
	return 0;
}

long ftell(FILE *stream) {
	off_t new_offset;
	if(mlibc::sys_seek(stream->fd, 0, SEEK_CUR, &new_offset))
		return EOF;
	return new_offset;
}

int fflush(FILE *stream) {
	if(stream->bufferBytes == 0)
		return 0;
	
	size_t written = 0;
	while(written < stream->bufferBytes) {
		ssize_t written_bytes;
		if(mlibc::sys_write(stream->fd, stream->bufferPtr + written,
				stream->bufferBytes - written, &written_bytes)) {
			stream->bufferBytes = 0;
			return EOF;
		}

		written += written_bytes;
		__ensure(written <= stream->bufferBytes);
	}

	stream->bufferBytes = 0;
	return 0;
}

int setvbuf(FILE *__restrict stream, char *__restrict buffer, int mode, size_t size) {
	__ensure(mode == _IOLBF);
	__ensure(stream->bufferBytes == 0);

	// TODO: free the old buffer
	if(!buffer) {
		auto new_buffer = (char *)malloc(size);
		__ensure(new_buffer);
		stream->bufferPtr = new_buffer;
		stream->bufferSize = size;
	}else{
		stream->bufferPtr = buffer;
		stream->bufferSize = size;
	}

	return 0;
}

