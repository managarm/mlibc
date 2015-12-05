
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <mlibc/ensure.h>

__mlibc_File stdinFile;
__mlibc_File stdoutFile;
__mlibc_File stderrFile;

FILE *stderr = &stderrFile;
FILE *stdin = &stdinFile;
FILE *stdout = &stdoutFile;

void __mlibc_initStdio() {
	size_t buffer_size = 4096;
	
	stdinFile.bufferPtr = (char *)malloc(buffer_size);
	stdinFile.bufferSize = buffer_size;

	stdoutFile.bufferPtr = (char *)malloc(buffer_size);
	stdoutFile.bufferSize = buffer_size;
	
	stderrFile.bufferPtr = (char *)malloc(buffer_size);
	stderrFile.bufferSize = buffer_size;
}

size_t fwrite(const void *__restrict buffer, size_t size, size_t count,
		FILE *__restrict stream) {
	for(size_t i = 0; i < count; i++) {
		void *block_ptr = (char *)buffer + i * size;
		if(size > stream->bufferSize) {
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
		}
	}
}

