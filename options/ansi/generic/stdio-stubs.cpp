
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
#include <ctype.h>

#include <bits/ensure.h>

#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/file-io.hpp>
#include <mlibc/sysdeps.hpp>

template<typename F>
struct PrintfAgent {
	PrintfAgent(F *formatter, frg::va_struct *vsp)
	: _formatter{formatter}, _vsp{vsp} { }

	void operator() (char c) {
		_formatter->append(c);
	}
	void operator() (const char *c, size_t n) {
		_formatter->append(c, n);
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
			__ensure(!opts.fill_zeros);
			__ensure(!opts.left_justify);
			__ensure(!opts.alt_conversion);
			__ensure(opts.minimum_width == 0);
			__ensure(szmod == frg::printf_size_mod::default_size);
			__ensure(!opts.precision);
			_formatter->append(strerror(errno));
			break;
		case 'n': {
			__ensure(szmod == frg::printf_size_mod::default_size);
			auto p = va_arg(_vsp->args, int *);
			*p = _formatter->count;
			break;
		}
		default:
			mlibc::infoLogger() << "\e[31mmlibc: Unknown printf terminator '"
					<< t << "'\e[39m" << frg::endlog;
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

	void append(const char *str, size_t n) {
		fwrite(str, n, 1, stream);
		count += n;
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

	void append(const char *str, size_t n) {
		// TODO: use strcat
		for(size_t i = 0; i < n; i++) {
			buffer[count] = str[i];
			count++;
		}
	}

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

	void append(const char *str, size_t n) {
		// TODO: use strcat
		for(size_t i = 0; i < n; i++)
			append(str[i]);
	}

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

	void append(const char *str, size_t n) {
		for(size_t i = 0; i < n; i++)
			append(str[i]);
	}

	char *buffer;
	size_t limit;
	size_t count;
};

int remove(const char *filename) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int rename(const char *path, const char *new_path) {
	if(!mlibc::sys_rename) {
		MLIBC_MISSING_SYSDEP();
		errno = ENOSYS;
		return -1;
	}
	if(int e = mlibc::sys_rename(path, new_path); e) {
		errno = e;
		return -1;
	}
	return 0;
}
int renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
	if(!mlibc::sys_renameat) {
        MLIBC_MISSING_SYSDEP();
        errno = ENOSYS;
        return -1;
    }
    if(int e = mlibc::sys_renameat(olddirfd, old_path, newdirfd, new_path); e) {
        errno = e;
        return -1;
    }
    return 0;
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
	setvbuf(stream, buffer, buffer ? _IOFBF : _IONBF, BUFSIZ);
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
	va_list args;
	va_start(args, format);
	int result = vfscanf(stream, format, args);
	va_end(args);
	return result;
}

int printf(const char *__restrict format, ...) {
	va_list args;
	va_start(args, format);
	int result = vfprintf(stdout, format, args);
	va_end(args);
	return result;
}

namespace {
    enum {
        SCANF_TYPE_CHAR,
        SCANF_TYPE_SHORT,
        SCANF_TYPE_INTMAX,
        SCANF_TYPE_L,
        SCANF_TYPE_LL,
        SCANF_TYPE_PTRDIFF,
        SCANF_TYPE_SIZE_T,
        SCANF_TYPE_INT
    };
}

static void store_int(void *dest, unsigned int size, unsigned long long i) {
    switch (size) {
        case SCANF_TYPE_CHAR:
            *(char *)dest = i;
            break;
        case SCANF_TYPE_SHORT:
            *(short *)dest = i;
            break;
        case SCANF_TYPE_INTMAX:
            *(intmax_t *)dest = i;
            break;
        case SCANF_TYPE_L:
            *(long *)dest = i;
            break;
        case SCANF_TYPE_LL:
            *(long long *)dest = i;
            break;
        case SCANF_TYPE_PTRDIFF:
            *(ptrdiff_t *)dest = i;
            break;
        case SCANF_TYPE_SIZE_T:
            *(size_t *)dest = i;
            break;
        /* fallthrough */
        case SCANF_TYPE_INT:
        default:
            *(int *)dest = i;
            break;
    }
}

template<typename H>
static int do_scanf(H &handler, const char *fmt, __gnuc_va_list args) {
    int match_count = 0;
    for (; *fmt; fmt++) {

        if (isspace(*fmt)) {
            while (isspace(fmt[1])) fmt++;
            while (isspace(handler.look_ahead()))
                    handler.consume();
            continue;
        }

        if (*fmt != '%' || fmt[1] == '%') {
            if (*fmt == '%')
                fmt++;
            char c = handler.consume();
            if (c != *fmt)
                break;
            continue;
        }

        void *dest = NULL;
        /* %n$ format */
        if (isdigit(*fmt) && fmt[1] == '$') {
            /* TODO: dest = get_arg_at_pos(args, *fmt -'0'); */
            fmt += 3;
        } else {
            dest = va_arg(args, void*);
            fmt++;
        }

        int width = 0;
        if (*fmt == '*') {
            fmt++;
            continue;
        } else if (*fmt == '\'') {
        /* TODO: numeric seperators locale stuff */
             mlibc::infoLogger() << "do_scanf: \' not implemented!" << frg::endlog;
            fmt++;
            continue;
        } else if (*fmt == 'm') {
            /* TODO: allocate buffer for them */
            mlibc::infoLogger() << "do_scanf: m not implemented!" << frg::endlog;
            fmt++;
            continue;
        } else if (*fmt >= '0' && *fmt <= '9') {
            /* read in width specifier */
            width = 0;
            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt - '0');
                fmt++;
                continue;
            }
        }

        /* type modifiers */
        unsigned int type = SCANF_TYPE_INT;
        unsigned int base = 10;
        switch (*fmt) {
            case 'h': {
                if (fmt[1] == 'h') {
                    type = SCANF_TYPE_CHAR;
                    fmt += 2;
                    break;
                }
                type = SCANF_TYPE_SHORT;
                fmt++;
                break;
            }
            case 'j': {
                type = SCANF_TYPE_INTMAX;
                fmt++;
                break;
            }
            case 'l': {
                if (fmt[1] == 'l') {
                    type = SCANF_TYPE_LL;
                    fmt += 2;
                    break;
                }
                type = SCANF_TYPE_L;
                fmt++;
                break;
            }
            case 'L': {
                type = SCANF_TYPE_LL;
                fmt++;
                break;
            }
            case 'q': {
                type = SCANF_TYPE_LL;
                fmt++;
                break;
            }
            case 't': {
                type = SCANF_TYPE_PTRDIFF;
                fmt++;
                break;
            }
             case 'z': {
                type = SCANF_TYPE_SIZE_T;
                fmt++;
                break;
            }
            case '0': {
                if (fmt[1] == 'x' || fmt[1] == 'X') {
                    base = 16;
                    fmt += 2;
                    break;
                }
                base = 8;
                fmt++;
                break;
            }
        }

        switch (*fmt) {
            case 'd':
            case 'u':
                base = 10;
                /* fallthrough */
            case 'i': {
                unsigned long long res = 0;
                char c = handler.look_ahead();
                switch (base) {
                    case 10:
                        while (c >= '0' && c <= '9') {
                            handler.consume();
                            res = res * 10 + (c - '0');
                            c = handler.look_ahead();
                        }
                        break;
                    case 16:
                        if (c == '0') {
                            handler.consume();
                            c = handler.look_ahead();
                            if (c == 'x') {
                                handler.consume();
                                c = handler.look_ahead();
                            }
                        }
                        while (true) {
                            if (c >= '0' && c <= '9') {
                                handler.consume();
                                res = res * 16 + (c - '0');
                            } else if (c >= 'a' && c <= 'f') {
                                handler.consume();
                                res = res * 16 + (c - 'a');
                            } else if (c >= 'A' && c <= 'F') {
                                handler.consume();
                                res = res * 16 + (c - 'A');
                            } else {
                                break;
                            }
                            c = handler.look_ahead();
                        }
                        break;
                    case 8:
                        while (c >= '0' && c <= '7') {
                            handler.consume();
                            res = res * 10 + (c - '0');
                            c = handler.look_ahead();
                        }
                        break;
                }
                if (dest)
                    store_int(dest, type, res);
                break;
            }
            case 'o': {
                unsigned long long res = 0;
                char c = handler.look_ahead();
                while (c >= '0' && c <= '7') {
                    handler.consume();
                    res = res * 10 + (c - '0');
                    c = handler.look_ahead();
                }
                if (dest)
                    store_int(dest, type, res);
                break;
            }
            case 'x':
            case 'X': {
                unsigned long long res = 0;
                char c = handler.look_ahead();
                if (c == '0') {
                    handler.consume();
                    c = handler.look_ahead();
                    if (c == 'x') {
                        handler.consume();
                        c = handler.look_ahead();
                    }
                }
                while (true) {
                    if (c >= '0' && c <= '9') {
                        handler.consume();
                        res = res * 16 + (c - '0');
                    } else if (c >= 'a' && c <= 'f') {
                        handler.consume();
                        res = res * 16 + (c - 'a');
                    } else if (c >= 'A' && c <= 'F') {
                        handler.consume();
                        res = res * 16 + (c - 'A');
                    } else {
                        break;
                    }
                    c = handler.look_ahead();
                }
                if (dest)
                    store_int(dest, type, res);
                break;
            }
            case 's': {
                char *typed_dest = (char *)dest;
                char c = handler.look_ahead();
                int count = 0;
                while (c && !isspace(c)) {
                    handler.consume();
                    if (typed_dest)
                        typed_dest[count] = c;
                    c = handler.look_ahead();
                    count++;
                    if (width && count >= width)
                        break;
                }
                if (typed_dest)
                    typed_dest[count + 1] = '\0';
                break;
            }
            case 'c': {
                char *typed_dest = (char *)dest;
                char c = handler.look_ahead();
                int count = 0;
                if (!width)
                    width = 1;
                while (c && count < width) {
                    handler.consume();
                    if (typed_dest)
                        typed_dest[count] = c;
                    c = handler.look_ahead();
                    count++;
                }
                break;
            }
            case '[': {
                fmt++;
                int invert = 0;
                if (*fmt == '^') {
                    invert = 1;
                    fmt++;
                }

                char scanset[257];
                memset(&scanset[0], invert, sizeof(char) * 257);
                scanset[0] = '\0';

                if (*fmt == '-') {
                    fmt++;
                    scanset[1+'-'] = 1 - invert;
                } else if (*fmt == ']') {
                    fmt++;
                    scanset[1+']'] = 1 - invert;
                }

                for (; *fmt != ']'; fmt++) {
                    if (!*fmt) return EOF;
                    if (*fmt == '-' && *fmt != ']') {
                        fmt++;
                        for (char c = *(fmt - 2); c < *fmt; c++)
                            scanset[1 + c] = 1 - invert;
                    }
                    scanset[1 + *fmt] = 1 - invert;
                }

                char *typed_dest = (char *)dest;
                int count = 0;
                char c = handler.look_ahead();
                while (c && count < width) {
                    handler.consume();
                    if (!scanset[1 + c])
                        break;
                    if (typed_dest)
                        typed_dest[count] = c;
                    c = handler.look_ahead();
                    count++;
                }
                if (typed_dest)
                    typed_dest[count] = '\0';
                break;
            }
            case 'p': {
                unsigned long long res = 0;
                char c = handler.look_ahead();
                if (c == '0') {
                    handler.consume();
                    c = handler.look_ahead();
                    if (c == 'x') {
                        handler.consume();
                        c = handler.look_ahead();
                    }
                }
                while (true) {
                    if (c >= '0' && c <= '9') {
                        handler.consume();
                        res = res * 16 + (c - '0');
                    } else if (c >= 'a' && c <= 'f') {
                        handler.consume();
                        res = res * 16 + (c - 'a');
                    } else if (c >= 'A' && c <= 'F') {
                        handler.consume();
                        res = res * 16 + (c - 'A');
                    } else {
                        break;
                    }
                    c = handler.look_ahead();
                }
                void **typed_dest = (void **)dest;
                *typed_dest = (void *)(uintptr_t)res;
                break;
            }
            case 'n': {
                int *typed_dest = (int *)dest;
                if (typed_dest)
                    *typed_dest = handler.num_consumed;
                continue;
            }
        }
        if (dest) match_count++;
    }
    return match_count;
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
    class {
    public:
        char look_ahead() {
            return *buffer;
        }

        char consume() {
            num_consumed++;
            return *buffer++;
        }

        const char *buffer;
        int num_consumed;
    } handler = {buffer};
    va_list args;
    va_start(args, format);
    int result = do_scanf(handler, format, args);
    va_end(args);
    return result;
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
	auto file = static_cast<mlibc::abstract_file *>(stream);

	struct {
		char look_ahead() {
			char c;
			size_t actual_size;
			file->read(&c, 1, &actual_size);
			if (actual_size)
				file->unget(c);
			return actual_size ? c : 0;
		}

		char consume() {
			char c;
			size_t actual_size;
			file->read(&c, 1, &actual_size);
			if (actual_size)
				num_consumed++;
			return actual_size ? c : 0;
		}

		mlibc::abstract_file *file;
		int num_consumed;
	} handler = {file};

	return do_scanf(handler, format, args);
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

int fwprintf(FILE *__restrict, const wchar_t *__restrict, ...) MLIBC_STUB_BODY
int fwscanf(FILE *__restrict, const wchar_t *__restrict, ...) MLIBC_STUB_BODY
int vfwprintf(FILE *__restrict, const wchar_t *__restrict, __gnuc_va_list) MLIBC_STUB_BODY
int vfwscanf(FILE *__restrict, const wchar_t *__restrict, __gnuc_va_list) MLIBC_STUB_BODY

int swprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict, ...) MLIBC_STUB_BODY
int swscanf(wchar_t *__restrict, size_t, const wchar_t *__restrict, ...) MLIBC_STUB_BODY
int vswprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict, __gnuc_va_list) MLIBC_STUB_BODY
int vswscanf(wchar_t *__restrict, size_t, const wchar_t *__restrict, __gnuc_va_list) MLIBC_STUB_BODY

int wprintf(const wchar_t *__restrict, ...) MLIBC_STUB_BODY
int wscanf(const wchar_t *__restrict, ...) MLIBC_STUB_BODY
int vwprintf(const wchar_t *__restrict, __gnuc_va_list) MLIBC_STUB_BODY
int vwscanf(const wchar_t *__restrict, __gnuc_va_list) MLIBC_STUB_BODY

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
		if (i == max_size - 1) {
			buffer[i] = 0;
			return buffer;
		}

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

		if(c == '\n') {
			buffer[i + 1] = 0;
			return buffer;
		}
	}
}

int fputc_unlocked(int c, FILE *stream) {
	char d = c;
	if(fwrite(&d, 1, 1, stream) != 1)
		return EOF;
	return 1;
}
int fputc(int c, FILE *stream) {
	return fputc_unlocked(c, stream);
}

int fputs_unlocked(const char *__restrict string, FILE *__restrict stream) {
	if(fwrite(string, strlen(string), 1, stream) != 1)
		return EOF;
	return 1;
}
int fputs(const char *__restrict string, FILE *__restrict stream) {
	return fputs_unlocked(string, stream);
}

int getc_unlocked(FILE *stream) {
	return fgetc_unlocked(stream);
}

int getc(FILE *stream) {
	return fgetc(stream);
}

int getchar_unlocked(void) {
	return fgetc_unlocked(stdin);
}

int getchar(void) {
	return fgetc(stdin);
}

int putc_unlocked(int c, FILE *stream) {
	char d = c;
	if(fwrite(&d, 1, 1, stream) != 1)
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
	auto file = static_cast<mlibc::abstract_file *>(stdout);

	size_t progress = 0;
	size_t len = strlen(string);
	while(progress < len) {
		size_t chunk;
		if(file->write(string + progress,
				len - progress, &chunk)) {
			return EOF;
		}else if(!chunk) {
			return EOF;
		}

		progress += chunk;
	}

	size_t unused;
	if (!file->write("\n", 1, &unused)) {
		return EOF;
	}

	return 1;
}

wint_t fgetwc(FILE *) MLIBC_STUB_BODY
wchar_t *fgetws(wchar_t *__restrict, int, FILE *__restrict) MLIBC_STUB_BODY
wint_t fputwc(wchar_t, FILE *) MLIBC_STUB_BODY
int fputws(const wchar_t *__restrict, FILE *__restrict) MLIBC_STUB_BODY
int fwide(FILE *, int) MLIBC_STUB_BODY
wint_t getwc(FILE *) MLIBC_STUB_BODY
wint_t getwchar(void) MLIBC_STUB_BODY
wint_t putwc(wchar_t, FILE *) MLIBC_STUB_BODY
wint_t putwchar(wchar_t) MLIBC_STUB_BODY
wint_t ungetwc(wint_t, FILE *) MLIBC_STUB_BODY

size_t fread(void *buffer, size_t size, size_t count, FILE *file_base) {
	return fread_unlocked(buffer, size, count, file_base);
}

size_t fwrite(const void *buffer, size_t size , size_t count, FILE *file_base) {
	return fwrite_unlocked(buffer, size, count, file_base);
}

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

void clearerr(FILE *file_base) {
	file_base->__status_bits = 0;
}

int feof(FILE *file_base) {
	return file_base->__status_bits & __MLIBC_EOF_BIT;
}

int ferror(FILE *file_base) {
	return file_base->__status_bits & __MLIBC_ERROR_BIT;
}

void perror(const char *string) {
	int error = errno;
	if (string && *string) {
		fprintf(stderr, "%s: ", string);
	}
	fprintf(stderr, "%s\n", strerror(error));
}

// POSIX unlocked I/O extensions.

// GLIBC extensions.

ssize_t getline(char **line, size_t *n, FILE *file_base) {
	// Otherwise, we cannot store the buffer / size.
	if(!line || !n) {
		errno = EINVAL;
		return -1;
	}

	char *buffer = nullptr;
	size_t capacity = 0;
	if(*line) {
		buffer = *line;
		capacity = *n;
	}

	if(!capacity) {
		buffer = reinterpret_cast<char *>(getAllocator().allocate(1024));
		capacity = 1024;
	}

	if(!fgets(buffer, capacity, file_base))
		return -1;

	size_t k = strlen(buffer);
	buffer[k] = '\n';
	buffer[k + 1] = 0;
	mlibc::infoLogger() << "returns: " << frg::escape_fmt(buffer, k + 1) << frg::endlog;

	*line = buffer;
	*n = capacity;
	return k + 1;
	//return getdelim(line, n, '\n', file_base);
}

ssize_t getdelim(char **line, size_t *n, int delim, FILE *file_base) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

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

void flockfile(FILE *) {
	mlibc::infoLogger() << "mlibc: File locking (flockfile) is a no-op" << frg::endlog;
}

void funlockfile(FILE *) {
	mlibc::infoLogger() << "mlibc: File locking (funlockfile) is a no-op" << frg::endlog;
}

int ftrylockfile(FILE *) {
	mlibc::infoLogger() << "mlibc: File locking (ftrylockfile) is a no-op" << frg::endlog;
}

void clearerr_unlocked(FILE *file_base) {
	file_base->__status_bits = 0;
}

int feof_unlocked(FILE *file_base) {
	return file_base->__status_bits & __MLIBC_EOF_BIT;
}

int ferror_unlocked(FILE *file_base) {
	return file_base->__status_bits & __MLIBC_ERROR_BIT;
}

int fgetc_unlocked(FILE *stream) {
	unsigned char d;
	if(fread(&d, 1, 1, stream) != 1)
		return EOF;
	return (int)d;
}

size_t fread_unlocked(void *buffer, size_t size, size_t count, FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	if(!size || !count)
		return 0;

	// Distinguish two cases here: If the object size is one, we perform byte-wise reads.
	// Otherwise, we try to read each object individually.
	if(size == 1) {
		size_t progress = 0;
		while(progress < count) {
			size_t chunk;
			if(file->read((char *)buffer + progress,
					count - progress, &chunk)) {
				// TODO: Handle I/O errors.
				mlibc::infoLogger() << "mlibc: fread() I/O errors are not handled"
						<< frg::endlog;
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
					mlibc::infoLogger() << "mlibc: fread() I/O errors are not handled"
							<< frg::endlog;
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

size_t fwrite_unlocked(const void *buffer, size_t size, size_t count, FILE *file_base) {
	auto file = static_cast<mlibc::abstract_file *>(file_base);
	if(!size || !count)
		return 0;

	// Distinguish two cases here: If the object size is one, we perform byte-wise writes.
	// Otherwise, we try to write each object individually.
	if(size == 1) {
		size_t progress = 0;
		while(progress < count) {
			size_t chunk;
			if(file->write((const char *)buffer + progress,
					count - progress, &chunk)) {
				// TODO: Handle I/O errors.
				mlibc::infoLogger() << "mlibc: fwrite() I/O errors are not handled"
						<< frg::endlog;
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
					mlibc::infoLogger() << "mlibc: fwrite() I/O errors are not handled"
							<< frg::endlog;
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

char *fgets_unlocked(char *, int, FILE *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

