#ifndef _WCTYPE_H
#define _WCTYPE_H

#include <bits/wint_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long wctype_t;
typedef unsigned long wctrans_t;

// [C11/7.30.2.2] Extensible wide character classification functions.

int iswalnum(wint_t);
int iswalpha(wint_t);
int iswblank(wint_t);
int iswcntrl(wint_t);
int iswdigit(wint_t);
int iswgraph(wint_t);
int iswlower(wint_t);
int iswprint(wint_t);
int iswpunct(wint_t);
int iswspace(wint_t);
int iswupper(wint_t);
int iswxdigit(wint_t);

wctype_t wctype(const char *);
int iswctype(wint_t, wctype_t);

// [C11/7.30.3] Wide character case mapping utilities.

wint_t towlower(wint_t);
wint_t towupper(wint_t);

wctrans_t wctrans(const char *);
wint_t towctrans(wint_t, wctrans_t);

#ifdef __cplusplus
}
#endif

#endif // _WCTYPE_H
