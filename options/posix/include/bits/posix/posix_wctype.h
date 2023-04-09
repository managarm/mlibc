#ifndef _POSIX_WCTYPE_H
#define _POSIX_WCTYPE_H

#include <bits/posix/locale_t.h>
#include <bits/wint_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int iswalnum_l(wint_t, locale_t);
int iswblank_l(wint_t, locale_t);
int iswcntrl_l(wint_t, locale_t);
int iswdigit_l(wint_t, locale_t);
int iswgraph_l(wint_t, locale_t);
int iswlower_l(wint_t, locale_t);
int iswprint_l(wint_t, locale_t);
int iswpunct_l(wint_t, locale_t);
int iswspace_l(wint_t, locale_t);
int iswupper_l(wint_t, locale_t);
int iswxdigit_l(wint_t, locale_t);
int iswalpha_l(wint_t, locale_t);

wctype_t wctype_l(const char *);
int iswctype_l(wint_t, wctype_t);

wint_t towlower_l(wint_t, locale_t);
wint_t towupper_l(wint_t, locale_t);

wctrans_t wctrans_l(const char *, locale_t);
wint_t towctrans_l(wint_t, wctrans_t, locale_t);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _POSIX_WCTYPE_H
