#ifndef _POSIX_WCTYPE_H
#define _POSIX_WCTYPE_H

#include <bits/posix/locale_t.h>
#include <bits/wint_t.h>
#include <bits/wctype_t.h>
#include <bits/wctrans_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int iswalnum_l(wint_t __wc, locale_t __loc);
int iswblank_l(wint_t __wc, locale_t __loc);
int iswcntrl_l(wint_t __wc, locale_t __loc);
int iswdigit_l(wint_t __wc, locale_t __loc);
int iswgraph_l(wint_t __wc, locale_t __loc);
int iswlower_l(wint_t __wc, locale_t __loc);
int iswprint_l(wint_t __wc, locale_t __loc);
int iswpunct_l(wint_t __wc, locale_t __loc);
int iswspace_l(wint_t __wc, locale_t __loc);
int iswupper_l(wint_t __wc, locale_t __loc);
int iswxdigit_l(wint_t __wc, locale_t __loc);
int iswalpha_l(wint_t __wc, locale_t __loc);

wctype_t wctype_l(const char *__string, locale_t __loc);
int iswctype_l(wint_t __wc, wctype_t __type, locale_t __loc);

wint_t towlower_l(wint_t __wc, locale_t __loc);
wint_t towupper_l(wint_t __wc, locale_t __loc);

wctrans_t wctrans_l(const char *__string, locale_t __loc);
wint_t towctrans_l(wint_t __wc, wctrans_t __trans, locale_t __loc);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _POSIX_WCTYPE_H */
