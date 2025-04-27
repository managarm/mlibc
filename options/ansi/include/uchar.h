#ifndef _UCHAR_H
#define _UCHAR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/mbstate.h>
#include <bits/size_t.h>

/* These are builtin types since C++11. */
#if !defined(__cplusplus) || __cplusplus < 201100L
typedef __CHAR16_TYPE__ char16_t;
typedef __CHAR32_TYPE__ char32_t;
#endif

#ifndef __MLIBC_ABI_ONLY

size_t c32rtomb(char *__restrict __s, char32_t __c32, mbstate_t *__restrict __ps);
size_t mbrtoc32(char32_t *__restrict __pc32, const char *__restrict __pmb, size_t __max, mbstate_t *__restrict __ps);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _UCHAR_H */
