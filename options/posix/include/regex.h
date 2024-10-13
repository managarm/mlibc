#ifndef _REGEX_H
#define _REGEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef ptrdiff_t regoff_t;

typedef struct re_pattern_buffer {
	size_t re_nsub;
	void *__opaque, *__padding[4];
	size_t __nsub2;
	char __padding2;
} regex_t;

typedef struct {
	regoff_t rm_so;
	regoff_t rm_eo;
} regmatch_t;

/* Flags for regcomp(). */
#define REG_EXTENDED 1
#define REG_ICASE 2
#define REG_NEWLINE 4
#define REG_NOSUB 8

/* Flags for regexec(). */
#define REG_NOTBOL 1
#define REG_NOTEOL 2

/* Errors for regcomp() and regexec(). */
#define REG_OK 0
#define REG_NOMATCH 1
#define REG_BADPAT 2
#define REG_ECOLLATE 3
#define REG_ECTYPE 4
#define REG_EESCAPE 5
#define REG_ESUBREG 6
#define REG_EBRACK 7
#define REG_EPAREN 8
#define REG_EBRACE 9
#define REG_BADBR 10
#define REG_ERANGE 11
#define REG_ESPACE 12
#define REG_BADRPT 13

/* Obsolete in POSIX. */
#define REG_ENOSYS -1

#ifndef __MLIBC_ABI_ONLY

int regcomp(regex_t *__restrict __regex, const char *__restrict __pattern, int __flags);
int regexec(const regex_t *__restrict __regex, const char *__restrict __string, size_t __nmatch,
		regmatch_t *__restrict __pmatch, int __flags);
size_t regerror(int __errcode, const regex_t *__restrict __regex, char *__restrict __errbuf, size_t __errbuf_size);
void regfree(regex_t *__regex);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif
