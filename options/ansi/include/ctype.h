#ifndef _CTYPE_H
#define _CTYPE_H

#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

/* Character classification function [7.4.1] */
int isalnum(int __c);
int isalpha(int __c);
int isblank(int __c);
int iscntrl(int __c);
int isdigit(int __c);
int isgraph(int __c);
int islower(int __c);
int isprint(int __c);
int ispunct(int __c);
int isspace(int __c);
int isupper(int __c);
int isxdigit(int __c);

/* glibc extensions. */
int isascii(int __c);

/* Character case mapping functions [7.4.2] */
int tolower(int __c);
int toupper(int __c);

#endif /* !__MLIBC_ABI_ONLY */

/* Borrowed from glibc */
#define	toascii(c)	((c) & 0x7f)

#ifdef __cplusplus
}
#endif

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_ctype.h>
#endif

#endif /* _CTYPE_H */
