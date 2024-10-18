#ifndef _WCTYPE_H
#define _WCTYPE_H

#include <mlibc-config.h>
#include <bits/wint_t.h>
#include <bits/wctype_t.h>
#include <bits/wctrans_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

/* [C11/7.30.2.2] Extensible wide character classification functions. */

int iswalnum(wint_t __wc);
int iswalpha(wint_t __wc);
int iswblank(wint_t __wc);
int iswcntrl(wint_t __wc);
int iswdigit(wint_t __wc);
int iswgraph(wint_t __wc);
int iswlower(wint_t __wc);
int iswprint(wint_t __wc);
int iswpunct(wint_t __wc);
int iswspace(wint_t __wc);
int iswupper(wint_t __wc);
int iswxdigit(wint_t __wc);

wctype_t wctype(const char *__string);
int iswctype(wint_t __wc, wctype_t __type);

/* [C11/7.30.3] Wide character case mapping utilities. */

wint_t towlower(wint_t __wc);
wint_t towupper(wint_t __wc);

wctrans_t wctrans(const char *__string);
wint_t towctrans(wint_t __wc, wctrans_t __trans);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_wctype.h>
#endif

#endif /* _WCTYPE_H */
