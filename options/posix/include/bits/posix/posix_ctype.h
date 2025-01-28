#ifndef _POSIX_CTYPE_H
#define _POSIX_CTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/posix/locale_t.h>

#ifndef __MLIBC_ABI_ONLY

int isalnum_l(int __c, locale_t __loc);
int isalpha_l(int __c, locale_t __loc);
int isblank_l(int __c, locale_t __loc);
int iscntrl_l(int __c, locale_t __loc);
int isdigit_l(int __c, locale_t __loc);
int isgraph_l(int __c, locale_t __loc);
int islower_l(int __c, locale_t __loc);
int isprint_l(int __c, locale_t __loc);
int ispunct_l(int __c, locale_t __loc);
int isspace_l(int __c, locale_t __loc);
int isupper_l(int __c, locale_t __loc);
int isxdigit_l(int __c, locale_t __loc);

int isascii_l(int __c, locale_t __loc);

int tolower_l(int __c, locale_t __loc);
int toupper_l(int __c, locale_t __loc); 

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_CTYPE_H */
