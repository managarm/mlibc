#ifndef _CTYPE_H
#define _CTYPE_H

#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

// Character classification function [7.4.1]
int isalnum(int c);
int isalpha(int c);
int isblank(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

// glibc extensions.
int isascii(int c);

// Character case mapping functions [7.4.2]
int tolower(int c);
int toupper(int c);

// Borrowed from glibc
#define	toascii(c)	((c) & 0x7f)

#ifdef __cplusplus
}
#endif

#ifdef __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_ctype.h>
#endif

#endif // _CTYPE_H
