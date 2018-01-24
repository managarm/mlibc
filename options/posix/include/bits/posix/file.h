#ifndef MLIBC_FILE_H
#define MLIBC_FILE_H

#include <bits/size_t.h>

struct __mlibc_File {
	// local buffer for I/O operations
	char *bufferPtr;
	
	// number of bytes the buffer can hold
	size_t bufferSize;
	
	// number of bytes that are currently buffered
	size_t bufferBytes;
	
	// underlying file descriptor
	int fd;
};

#endif // MLIBC_FILE_H
