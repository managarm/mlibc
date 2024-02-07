
#ifndef _STDIO_H
#define _STDIO_H

#include <abi-bits/seek-whence.h>
#include <bits/null.h>
#include <bits/size_t.h>
#include <mlibc-config.h>

// Glibc extensions require ssize_t.
#include <bits/ssize_t.h>

#ifdef __cplusplus
extern "C" {
#endif

// [C11-7.21.1] I/O related types

#define __MLIBC_EOF_BIT 1
#define __MLIBC_ERROR_BIT 2

struct __mlibc_file_base {
	// Buffer for I/O operations.
	// We reserve a few extra bytes for ungetc operations. This means
	// that __buffer_ptr will point a few bytes *into* the allocation.
	char *__buffer_ptr;

	// Number of bytes the buffer can hold.
	size_t __buffer_size;

	// Current offset inside the buffer.
	size_t __offset;

	// Position inside the buffer that matches the current file pointer.
	size_t __io_offset;

	// Valid region of the buffer.
	size_t __valid_limit;

	// Begin and end of the dirty region inside the buffer.
	size_t __dirty_begin;
	size_t __dirty_end;

	// This points to the same place as __buffer_ptr, or a few bytes earlier
	// if there are bytes pushed by ungetc. If buffering is disabled, calls
	// to ungetc will trigger an allocation.
	char *__unget_ptr;

	// 0 if we are currently reading from the buffer.
	// 1 if we are currently writing to the buffer.
	// This is only really important for pipe-like streams.
	int __io_mode;

	// EOF and error bits.
	int __status_bits;
};

typedef struct __mlibc_file_base FILE;
typedef size_t fpos_t;

// [C11-7.21.1] I/O related macros

#define _IOFBF 1
#define _IOLBF 2
#define _IONBF 3

#define BUFSIZ 512

#define EOF (-1)

#define FOPEN_MAX 1024
#define FILENAME_MAX 256
#define L_tmpnam 256

#define TMP_MAX 1024

#ifndef __MLIBC_ABI_ONLY

extern FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;

// [C11-7.21.4] Operations on files

int remove(const char *filename);
int rename(const char *old_path, const char *new_path);
int renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path);
FILE *tmpfile(void);
char *tmpnam(char *buffer);

// [C11-7.21.5] File access functions

int fclose(FILE *stream);
int fflush(FILE *stream);
FILE *fopen(const char *__restrict filename, const char *__restrict mode);
FILE *freopen(const char *__restrict filename, const char *__restrict mode, FILE *__restrict stream);
void setbuf(FILE *__restrict stream, char *__restrict buffer);
int setvbuf(FILE *__restrict stream, char *__restrict buffer, int mode, size_t size);
void setlinebuf(FILE *stream);
void setbuffer(FILE *, char *, size_t);

// [C11-7.21.6] Formatted input/output functions

__attribute__((__format__(printf, 2, 3)))
int fprintf(FILE *__restrict stream, const char *__restrict format, ...);

__attribute__((__format__(scanf, 2, 3)))
int fscanf(FILE *__restrict stream, const char *__restrict format, ...);

__attribute__((__format__(printf, 1, 2)))
int printf(const char *__restrict format, ...);

__attribute__((__format__(scanf, 1, 2)))
int scanf(const char *__restrict format, ...);

__attribute__((__format__(printf, 3, 4)))
int snprintf(char *__restrict buffer, size_t max_size, const char *__restrict format, ...);

__attribute__((__format__(printf, 2, 3)))
int sprintf(char *__restrict buffer, const char *__restrict format, ...);

__attribute__((__format__(scanf, 2, 3)))
int sscanf(const char *__restrict buffer, const char *__restrict format, ...);

__attribute__((__format__(printf, 2, 0)))
int vfprintf(FILE *__restrict stream, const char *__restrict format, __builtin_va_list args);

__attribute__((__format__(scanf, 2, 0)))
int vfscanf(FILE *__restrict stream, const char *__restrict format, __builtin_va_list args);

__attribute__((__format__(printf, 1, 0)))
int vprintf(const char *__restrict format, __builtin_va_list args);

__attribute__((__format__(scanf, 1, 0)))
int vscanf(const char *__restrict format, __builtin_va_list args);

__attribute__((__format__(printf, 3, 0)))
int vsnprintf(char *__restrict buffer, size_t max_size,
		const char *__restrict format, __builtin_va_list args);

__attribute__((__format__(printf, 2, 0)))
int vsprintf(char *__restrict buffer, const char *__restrict format, __builtin_va_list args);

__attribute__((__format__(scanf, 2, 0)))
int vsscanf(const char *__restrict buffer, const char *__restrict format, __builtin_va_list args);

// this is a gnu extension
__attribute__((__format__(printf, 2, 0)))
int vasprintf(char **, const char *, __builtin_va_list);

// [C11-7.21.7] Character input/output functions

int fgetc(FILE *stream);
char *fgets(char *__restrict buffer, int max_size, FILE *__restrict stream);
int fputc(int c, FILE *stream);
int fputs(const char *__restrict string, FILE *__restrict stream);
char *gets(char *s);
int getc(FILE *stream);
int getchar(void);
int putc(int c, FILE *stream);
int putchar(int c);
int puts(const char *string);
int ungetc(int c, FILE *stream);

// [C11-7.21.8] Direct input/output functions

size_t fread(void *__restrict buffer, size_t size, size_t count, FILE *__restrict stream);
size_t fwrite(const void *__restrict buffer, size_t size, size_t count, FILE *__restrict stream);

// [C11-7.21.9] File positioning functions

int fgetpos(FILE *__restrict stream, fpos_t *__restrict position);
int fseek(FILE *stream, long offset, int whence);
int fsetpos(FILE *stream, const fpos_t *position);
long ftell(FILE *stream);
void rewind(FILE *stream);

// [C11-7.21.10] Error handling functions

void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *string);

// POSIX unlocked I/O extensions.

int getc_unlocked(FILE *);
int getchar_unlocked(void);
int putc_unlocked(int, FILE *);
int putchar_unlocked(int);

// GLIBC extensions.

ssize_t getline(char **, size_t *, FILE *);
ssize_t getdelim(char **, size_t *, int, FILE *);

int asprintf(char **, const char *, ...);

// Linux unlocked I/O extensions.

void flockfile(FILE *);
void funlockfile(FILE *);
int ftrylockfile(FILE *);

void clearerr_unlocked(FILE *);
int feof_unlocked(FILE *);
int ferror_unlocked(FILE *);
int fileno_unlocked(FILE *);
int fflush_unlocked(FILE *);
int fgetc_unlocked(FILE *);
int fputc_unlocked(int, FILE *);
size_t fread_unlocked(void *__restrict, size_t, size_t, FILE *__restrict);
size_t fwrite_unlocked(const void *__restrict, size_t, size_t, FILE *__restrict);

char *fgets_unlocked(char *, int, FILE *);
int fputs_unlocked(const char *, FILE *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_stdio.h>
#endif

#endif // _STDIO_H

