#ifndef _POSIX_CTYPE_H
#define _POSIX_CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/posix/locale_t.h>

int isalnum_l(int c, locale_t loc);
int isalpha_l(int c, locale_t loc);
int isblank_l(int c, locale_t loc);
int iscntrl_l(int c, locale_t loc);
int isdigit_l(int c, locale_t loc);
int isgraph_l(int c, locale_t loc);
int islower_l(int c, locale_t loc);
int isprint_l(int c, locale_t loc);
int ispunct_l(int c, locale_t loc);
int isspace_l(int c, locale_t loc);
int isupper_l(int c, locale_t loc);
int isxdigit_l(int c, locale_t loc);

int isascii_l(int c, locale_t loc);

int tolower_l(int c, locale_t loc);
int toupper_l(int c, locale_t loc); 

#ifdef __cplusplus
}
#endif

#endif // _POSIX_CTYPE_H