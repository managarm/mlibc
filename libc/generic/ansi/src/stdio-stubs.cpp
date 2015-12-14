
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// TODO: we need this for frigg::printf; replace this mechanism
#include <assert.h>

#include <mlibc/ensure.h>

#pragma GCC visibility push(hidden)

#include <frigg/debug.hpp>
#include <frigg/printf.hpp>

struct StreamPrinter {
	StreamPrinter(FILE *stream)
	: stream(stream) { }

	void print(char c) {
		fwrite(&c, 1, 1, stream);
	}

	void print(const char *str) {
		fwrite(str, strlen(str), 1, stream);
	}

	void flush() {
		fflush(stream);
	}

	FILE *stream;
};

struct BufferPrinter {
	BufferPrinter(char *buffer, size_t limit)
	: buffer(buffer), offset(0), limit(limit) { }

	void print(char c) {
		if(limit && !(offset < limit))
			return;
		buffer[offset] = c;
		offset++;
	}

	void print(const char *str) {
		// TODO: use strcat
		for(size_t i = 0; str[i]; i++) {
			if(limit && !(offset < limit))
				return;
			buffer[offset] = str[i];
			offset++;
		}
	}

	void flush() { }

	char *buffer;
	size_t offset;
	size_t limit;
};

#pragma GCC visibility pop

int remove(const char *filename) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int rename(const char *old_path, const char *new_path) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
FILE *tmpfile(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *tmpnam(char *buffer) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fclose(FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// fflush is provided by the POSIX sublibrary
FILE *fopen(const char *__restrict filename, const char *__restrict mode) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
FILE *freopen(const char *__restrict filename, const char *__restrict mode, FILE *__restrict stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void setbuf(FILE *__restrict stream, char *__restrict buffer) {
	__ensure(!"Not implemented");
}
// setvbuf() is provided by the POSIX sublibrary

int fprintf(FILE *__restrict stream, const char *__restrict format, ...) {
	va_list args;
	va_start(args, format);
	int result = vfprintf(stream, format, args);
	va_end(args);
	return result;
}
int fscanf(FILE *__restrict stream, const char *__restrict format, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int printf(const char *__restrict format, ...) {
	va_list args;
	va_start(args, format);
	int result = vfprintf(stdout, format, args);
	va_end(args);
	return result;
}
int scanf(const char *__restrict format, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int snprintf(char *__restrict buffer, size_t max_size, const char *__restrict format, ...) {
	va_list args;
	va_start(args, format);
	int result = vsnprintf(buffer, max_size, format, args);
	va_end(args);
	return result;
}
int sprintf(char *__restrict buffer, const char *__restrict format, ...) {
	va_list args;
	va_start(args, format);
	int result = vsprintf(buffer, format, args);
	va_end(args);
	return result;
}
int vfprintf(FILE *__restrict stream, const char *__restrict format, __gnuc_va_list args) {
	StreamPrinter p(stream);
	frigg::printf(p, format, args);
	return 0;
}
int vfscanf(FILE *__restrict stream, const char *__restrict format, __gnuc_va_list args) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int vprintf(const char *__restrict format, __gnuc_va_list args){
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int vscanf(const char *__restrict format, __gnuc_va_list args) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int vsnprintf(char *__restrict buffer, size_t max_size,
		const char *__restrict format, __gnuc_va_list args) {
	if(!max_size)
		return 0;
	BufferPrinter p(buffer, max_size - 1);
	frigg::printf(p, format, args);
	p.buffer[p.offset] = 0;
	return 0;
}
int vsprintf(char *__restrict buffer, const char *__restrict format, __gnuc_va_list args) {
	BufferPrinter p(buffer, 0);
	frigg::printf(p, format, args);
	p.buffer[p.offset] = 0;
	return 0;
}
int vsscanf(const char *__restrict buffer, const char *__restrict format, __gnuc_va_list args) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fgetc(FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *fgets(char *__restrict buffer, size_t max_size, FILE *__restrict stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fputc(int c, FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fputs(const char *__restrict string, FILE *__restrict stream) {
	if(fwrite(string, strlen(string), 1, stream) != 1)
		return EOF;
	if(fwrite("\n", 1, 1, stream) != 1)
		return EOF;
	if(fflush(stream))
		return EOF;
	return 1;
}
int getc(FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getchar(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int putc(int c, FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int putchar(int c) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int puts(const char *string) {
	return fputs(string, stdout);
}
int ungetc(int c, FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

size_t fread(void *__restrict buffer, size_t max_size, size_t count, FILE *__restrict stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// fwrite() is provided by the POSIX sublibrary

int fgetpos(FILE *__restrict stream, fpos_t *__restrict position) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fseek(FILE *stream, long offset, int whence) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fsetpos(FILE *stream, const fpos_t *position) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long ftell(FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void rewind(FILE *stream) {
	__ensure(!"Not implemented");
}

void clearerr(FILE *stream) {
	__ensure(!"Not implemented");
}
int feof(FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int ferror(FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int perror(const char *string) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

