
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <mlibc/ensure.h>

#pragma GCC visibility push(hidden)

#include <frigg/debug.hpp>

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

#pragma GCC visibility pop

FILE *stderr = &stderrFile;
FILE *stdin = &stdinFile;
FILE *stdout = &stdoutFile;

static bool disableBuffering = true;

FILE *fopen(const char *__restrict filename, const char *__restrict mode) {
	int fd;

	if(strcmp(mode, "r")) {
		fd = open(filename, O_RDONLY);
		if(fd == -1)
			return nullptr;
	}else{
		frigg::panicLogger.log() << "Illegal fopen() mode '" << mode << "'" << frigg::EndLog();
	}

	FILE *file = (FILE *)malloc(sizeof(FILE));
	file->fd = fd;
	file->bufferPtr = nullptr;
	file->bufferSize = 0;
	return file;
}

size_t fwrite(const void *__restrict buffer, size_t size, size_t count,
		FILE *__restrict stream) {
	for(size_t i = 0; i < count; i++) {
		char *block_ptr = (char *)buffer + i * size;
		if(size > stream->bufferSize || disableBuffering) {
			// write the buffer directly to the fd
			size_t written = 0;
			while(written < size) {
				ssize_t result = write(stream->fd, block_ptr + written, size - written);
				if(result < 0)
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
	if(lseek(stream->fd, offset, whence) != off_t(-1))
		return 0;
	return -1;
}

int fflush(FILE *stream) {
	if(stream->bufferBytes == 0)
		return 0;
	
	size_t written = 0;
	while(written < stream->bufferBytes) {
		ssize_t result = write(stream->fd, stream->bufferPtr + written,
				stream->bufferBytes - written);
		if(result < 0) {
			stream->bufferBytes = 0;
			return EOF;
		}

		written += result;
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

