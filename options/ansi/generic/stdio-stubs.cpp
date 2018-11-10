
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <bits/ensure.h>

#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

template<typename F>
struct PrintfAgent {
	PrintfAgent(F *formatter, frg::va_struct *vsp)
	: _formatter{formatter}, _vsp{vsp} { }

	void operator() (char c) {
		_formatter->append(c);
	}

	void operator() (char t, frg::format_options opts, frg::printf_size_mod szmod) {
		switch(t) {
		case 'p': case 'c': case 's':
			frg::do_printf_chars(*_formatter, t, opts, szmod, _vsp);
			break;
		case 'd': case 'i': case 'o': case 'x': case 'X': case 'u':
			frg::do_printf_ints(*_formatter, t, opts, szmod, _vsp);
			break;
		case 'f': case 'F': case 'g': case 'G': case 'e': case 'E':
			frg::do_printf_floats(*_formatter, t, opts, szmod, _vsp);
			break;
		case 'm':
			_formatter->append("%m");
			break;
		default:
			__ensure(!"Illegal printf terminator");
		}
	}

private:
	F *_formatter;
	frg::va_struct *_vsp;
};

struct StreamPrinter {
	StreamPrinter(FILE *stream)
	: stream(stream), count(0) { }

	void append(char c) {
		fwrite(&c, 1, 1, stream);
		count++;
	}

	void append(const char *str) {
		fwrite(str, strlen(str), 1, stream);
		count += strlen(str);
	}

	void flush() {
		fflush(stream);
	}

	FILE *stream;
	size_t count;
};

struct BufferPrinter {
	BufferPrinter(char *buffer)
	: buffer(buffer), count(0) { }

	void append(char c) {
		buffer[count] = c;
		count++;
	}

	void append(const char *str) {
		// TODO: use strcat
		for(size_t i = 0; str[i]; i++) {
			buffer[count] = str[i];
			count++;
		}
	}

	void flush() { }

	char *buffer;
	size_t count;
};

struct LimitedPrinter {
	LimitedPrinter(char *buffer, size_t limit)
	: buffer(buffer), limit(limit), count(0) { }

	void append(char c) {
		if(count < limit)
			buffer[count] = c;
		count++;
	}

	void append(const char *str) {
		// TODO: use strcat
		for(size_t i = 0; str[i]; i++)
			append(str[i]);
	}

	void flush() { }

	char *buffer;
	size_t limit;
	size_t count;
};

struct ResizePrinter {
	ResizePrinter()
	: buffer(nullptr), limit(0), count(0) { }

	void expand() {
		if(count == limit) {
			auto new_limit = frg::max(2 * limit, size_t(16));
			auto new_buffer = reinterpret_cast<char *>(malloc(new_limit));
			__ensure(new_buffer);
			memcpy(new_buffer, buffer, count);
			free(buffer);
			buffer = new_buffer;
			limit = new_limit;
		}
		__ensure(count < limit);
	}

	void append(char c) {
		expand();
		buffer[count] = c;
		count++;
	}

	void append(const char *str) {
		for(size_t i = 0; str[i]; i++)
			append(str[i]);
	}

	void flush() { }

	char *buffer;
	size_t limit;
	size_t count;
};

int remove(const char *filename) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int rename(const char *path, const char *new_path) {
	if(mlibc::sys_rename(path, new_path))
		return -1;
	return 0;
}
int renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
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

// fflush() is provided by the POSIX sublibrary
// fopen() is provided by the POSIX sublibrary
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
int sscanf(const char *__restrict buffer, const char *__restrict format, ...) {
	mlibc::infoLogger() << "sscanf(" << buffer << ", " << format << ")" << frg::endlog;
	return EOF;
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int vfprintf(FILE *__restrict stream, const char *__restrict format, __gnuc_va_list args) {
	frg::va_struct vs;
	va_copy(vs.args, args);
	StreamPrinter p{stream};
//	mlibc::infoLogger() << "printf(" << format << ")" << frg::endlog;
	frg::printf_format(PrintfAgent{&p, &vs}, format, &vs);
	return p.count;
}
int vfscanf(FILE *__restrict stream, const char *__restrict format, __gnuc_va_list args) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int vprintf(const char *__restrict format, __gnuc_va_list args){
	return vfprintf(stdout, format, args);
}
int vscanf(const char *__restrict format, __gnuc_va_list args) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int vsnprintf(char *__restrict buffer, size_t max_size,
		const char *__restrict format, __gnuc_va_list args) {
	if(!max_size)
		return 0;
	frg::va_struct vs;
	va_copy(vs.args, args);
	LimitedPrinter p{buffer, max_size - 1};
//	mlibc::infoLogger() << "printf(" << format << ")" << frg::endlog;
	frg::printf_format(PrintfAgent{&p, &vs}, format, &vs);
	p.buffer[frg::min(max_size - 1, p.count)] = 0;
	return p.count;
}
int vsprintf(char *__restrict buffer, const char *__restrict format, __gnuc_va_list args) {
	frg::va_struct vs;
	va_copy(vs.args, args);
	BufferPrinter p(buffer);
//	mlibc::infoLogger() << "printf(" << format << ")" << frg::endlog;
	frg::printf_format(PrintfAgent{&p, &vs}, format, &vs);
	p.buffer[p.count] = 0;
	return p.count;
}
int vsscanf(const char *__restrict buffer, const char *__restrict format, __gnuc_va_list args) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int fgetc(FILE *stream) {
	char c;
	auto bytes_read = fread(&c, 1, 1, stream);
	if(bytes_read != 1)
		return EOF;
	return c;
}

char *fgets(char *__restrict buffer, size_t max_size, FILE *__restrict stream) {
	__ensure(max_size > 0);
	for(size_t i = 0; ; i++) {
		auto c = fgetc(stream);
		
		// If fgetc() fails, there is either an EOF or an I/O error.
		if(c == EOF) {
//			if(ferror(stream)) {
//				// Technically, we do not have to terminate the buffer in this case;
//				// do it anyway to avoid UB if apps do not check our result.
//				buffer[i] = 0;
//				return nullptr;
//			}

			// EOF is only an error if no chars have been read yet.
			//__ensure(feof(stream));
			if(i) {
				buffer[i] = 0;
				return buffer;
			}else{
				// In this case, the buffer is not changed.
				return nullptr;
			}
		}else{
			buffer[i] = c;
		}

		if(c == '\n' || i == max_size - 1) {
			buffer[i + 1] = 0;
			return buffer;
		}
	}
}

int fputc_unlocked(int c, FILE *stream) {
	char d = c;
	if(fwrite(&d, 1, 1, stream) != 1)
		return EOF;
	if(fflush(stream))
		return EOF;
	return 1;
}
int fputc(int c, FILE *stream) {
	return fputc_unlocked(c, stream);
}

int fputs_unlocked(const char *__restrict string, FILE *__restrict stream) {
	if(fwrite(string, strlen(string), 1, stream) != 1)
		return EOF;
	if(fflush(stream))
		return EOF;
	return 1;
}
int fputs(const char *__restrict string, FILE *__restrict stream) {
	return fputs_unlocked(string, stream);
}

int getc(FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getchar(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int putc_unlocked(int c, FILE *stream) {
	char d = c;
	if(fwrite(&d, 1, 1, stream) != 1)
		return EOF;
	if(fflush(stream))
		return EOF;
	return c;
}
int putc(int c, FILE *stream) {
	return putc_unlocked(c, stream);
}

int putchar_unlocked(int c) {
	return putc_unlocked(c, stdout);
}
int putchar(int c) {
	return putchar_unlocked(c);
}

int puts(const char *string) {
	if(fwrite(string, strlen(string), 1, stdout) != 1)
		return EOF;
	if(fwrite("\n", 1, 1, stdout) != 1)
		return EOF;
	if(fflush(stdout))
		return EOF;
	return 1;
}
int ungetc(int c, FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// fread() is provided by the POSIX sublibrary

int fgetpos(FILE *__restrict stream, fpos_t *__restrict position) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// fseek() is provided by the POSIX sublibrary
int fsetpos(FILE *stream, const fpos_t *position) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// ftell() is provided by the POSIX sublibrary

void clearerr(FILE *stream) {
	// TODO: implement stdio error handling
}
int feof(FILE *stream) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int ferror(FILE *stream) {
	// TODO: implement stdio error handling
	return 0;
}
int perror(const char *string) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// POSIX unlocked I/O extensions.

int getc_unlocked(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getchar_unlocked(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// GLIBC extensions.
int asprintf(char **out, const char *format, ...) {
	va_list args;
	va_start(args, format);
	int result = vasprintf(out, format, args);
	va_end(args);
	return result;
}

int vasprintf(char **out, const char *format, __gnuc_va_list args) {
	frg::va_struct vs;
	va_copy(vs.args, args);
	ResizePrinter p;
//	mlibc::infoLogger() << "printf(" << format << ")" << frg::endlog;
	frg::printf_format(PrintfAgent{&p, &vs}, format, &vs);
	p.expand();
	p.buffer[p.count] = 0;
	*out = p.buffer;
	return p.count;
}

// Linux unlocked I/O extensions.

void clearerr_unlocked(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int feof_unlocked(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int ferror_unlocked(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fileno_unlocked(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fgetc_unlocked(FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
size_t fread_unlocked(void *, size_t, size_t, FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *fgets_unlocked(char *, int, FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

