
#ifndef _STDIO_H
#define _STDIO_H

#include <abi-bits/seek-whence.h>
#include <bits/null.h>
#include <bits/file.h>
#include <bits/size_t.h>
#include <mlibc-config.h>

/* Glibc extensions require ssize_t. */
#include <bits/ssize_t.h>
#include <bits/off_t.h>

#ifdef __cplusplus
extern "C" {
#endif

/* [C11-7.21.1] I/O related types */

#define __MLIBC_EOF_BIT 1
#define __MLIBC_ERROR_BIT 2

struct __mlibc_file_base {
	/* Buffer for I/O operations. */
	/* We reserve a few extra bytes for ungetc operations. This means */
	/* that __buffer_ptr will point a few bytes *into* the allocation. */
	char *__buffer_ptr;

	/* Number of bytes the buffer can hold. */
	size_t __buffer_size;

	/* Current offset inside the buffer. */
	size_t __offset;

	/* Position inside the buffer that matches the current file pointer. */
	size_t __io_offset;

	/* Valid region of the buffer. */
	size_t __valid_limit;

	/* Begin and end of the dirty region inside the buffer. */
	size_t __dirty_begin;
	size_t __dirty_end;

	/* This points to the same place as __buffer_ptr, or a few bytes earlier */
	/* if there are bytes pushed by ungetc. If buffering is disabled, calls */
	/* to ungetc will trigger an allocation. */
	char *__unget_ptr;

	/* 0 if we are currently reading from the buffer. */
	/* 1 if we are currently writing to the buffer. */
	/* This is only really important for pipe-like streams. */
	int __io_mode;

	/* EOF and error bits. */
	int __status_bits;
};

typedef off_t fpos_t;

/* [C11-7.21.1] I/O related macros */

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

/* [C11-7.21.4] Operations on files */

int remove(const char *__filename);
int rename(const char *__old_path, const char *__new_path);
int renameat(int __olddirfd, const char *__old_path, int __newdirfd, const char *__new_path);
FILE *tmpfile(void);
char *tmpnam(char *__buffer);

/* [C11-7.21.5] File access functions */

int fclose(FILE *__stream);
int fflush(FILE *__stream);
FILE *fopen(const char *__restrict __filename, const char *__restrict __mode);
FILE *freopen(const char *__restrict __filename, const char *__restrict __mode, FILE *__restrict __stream);
void setbuf(FILE *__restrict __stream, char *__restrict __buffer);
int setvbuf(FILE *__restrict __stream, char *__restrict __buffer, int __mode, size_t __size);
void setlinebuf(FILE *__stream);
void setbuffer(FILE *__stream, char *__buffer, size_t __size);

/* [C11-7.21.6] Formatted input/output functions */

__attribute__((__format__(__printf__, 2, 3)))
int fprintf(FILE *__restrict __stream, const char *__restrict __format, ...);

__attribute__((__format__(__scanf__, 2, 3)))
int fscanf(FILE *__restrict __stream, const char *__restrict __format, ...);

__attribute__((__format__(__printf__, 1, 2)))
int printf(const char *__restrict __format, ...);

__attribute__((__format__(__scanf__, 1, 2)))
int scanf(const char *__restrict __format, ...);

__attribute__((__format__(__printf__, 3, 4)))
int snprintf(char *__restrict __buffer, size_t __max_size, const char *__restrict __format, ...);

__attribute__((__format__(__printf__, 2, 3)))
int sprintf(char *__restrict __buffer, const char *__restrict __format, ...);

__attribute__((__format__(__scanf__, 2, 3)))
int sscanf(const char *__restrict __buffer, const char *__restrict __format, ...);

__attribute__((__format__(__printf__, 2, 0)))
int vfprintf(FILE *__restrict __stream, const char *__restrict __format, __builtin_va_list __args);

__attribute__((__format__(__scanf__, 2, 0)))
int vfscanf(FILE *__restrict __stream, const char *__restrict __format, __builtin_va_list __args);

__attribute__((__format__(__printf__, 1, 0)))
int vprintf(const char *__restrict __format, __builtin_va_list __args);

__attribute__((__format__(__scanf__, 1, 0)))
int vscanf(const char *__restrict __format, __builtin_va_list __args);

__attribute__((__format__(__printf__, 3, 0)))
int vsnprintf(char *__restrict __buffer, size_t __max_size,
		const char *__restrict __format, __builtin_va_list __args);

__attribute__((__format__(__printf__, 2, 0)))
int vsprintf(char *__restrict __buffer, const char *__restrict __format, __builtin_va_list __args);

__attribute__((__format__(__scanf__, 2, 0)))
int vsscanf(const char *__restrict __buffer, const char *__restrict __format, __builtin_va_list __args);

/* this is a gnu extension */
__attribute__((__format__(__printf__, 2, 0)))
int vasprintf(char **__buffer, const char *__format, __builtin_va_list __args);

/* [C11-7.21.7] Character input/output functions */

int fgetc(FILE *__stream);
char *fgets(char *__restrict __buffer, int __max_size, FILE *__restrict __stream);
int fputc(int __c, FILE *__stream);
int fputs(const char *__restrict __string, FILE *__restrict __stream);
char *gets(char *__s);
int getc(FILE *__stream);
int getchar(void);
int putc(int __c, FILE *__stream);
int putchar(int __c);
int puts(const char *__string);
int ungetc(int __c, FILE *__stream);

/* [C11-7.21.8] Direct input/output functions */

size_t fread(void *__restrict __buffer, size_t __size, size_t __count, FILE *__restrict __stream);
size_t fwrite(const void *__restrict __buffer, size_t __size, size_t __count, FILE *__restrict __stream);

/* [C11-7.21.9] File positioning functions */

int fgetpos(FILE *__restrict __stream, fpos_t *__restrict __position);
int fseek(FILE *__stream, long __offset, int __whence);
int fsetpos(FILE *__stream, const fpos_t *__position);
long ftell(FILE *__stream);
void rewind(FILE *__stream);

/* [C11-7.21.10] Error handling functions */

void clearerr(FILE *__stream);
int feof(FILE *__stream);
int ferror(FILE *__stream);
void perror(const char *__string);

/* POSIX unlocked I/O extensions. */

int getc_unlocked(FILE *__stream);
int getchar_unlocked(void);
int putc_unlocked(int __c, FILE *__stream);
int putchar_unlocked(int __c);

/* GLIBC extensions. */

ssize_t getline(char **__linep, size_t *__sizep, FILE *__stream);
ssize_t getdelim(char **__linep, size_t *__sizep, int __delim, FILE *__stream);

__attribute__((__format__(__printf__, 2, 3)))
int asprintf(char **__buffer, const char *__format, ...);

/* Linux unlocked I/O extensions. */

void flockfile(FILE *__stream);
void funlockfile(FILE *__stream);
int ftrylockfile(FILE *__stream);

void clearerr_unlocked(FILE *__stream);
int feof_unlocked(FILE *__stream);
int ferror_unlocked(FILE *__stream);
int fileno_unlocked(FILE *__stream);
int fflush_unlocked(FILE *__stream);
int fgetc_unlocked(FILE *__stream);
int fputc_unlocked(int __c, FILE *__stream);
size_t fread_unlocked(void *__restrict __buffer, size_t __size, size_t __count, FILE *__restrict __stream);
size_t fwrite_unlocked(const void *__restrict __buffer, size_t __size, size_t __count, FILE *__restrict __stream);

char *fgets_unlocked(char *__restrict __buffer, int __size, FILE *__restrict __stream);
int fputs_unlocked(const char *__restrict __buffer, FILE *__restrict __stream);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_stdio.h>
#endif

#endif /* _STDIO_H */

