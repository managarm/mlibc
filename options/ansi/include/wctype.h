#ifndef _WCTYPE_H
#define _WCTYPE_H

//#include <bits/null.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int wint_t;
typedef unsigned long wctype_t;

// [C11/7.30.2.2] Extensible wide character classification functions.

wctype_t wctype(const char *);
int iswctype(wint_t, wctype_t);

// [C11/7.30.3] Wide character case mapping utilities.

wint_t towlower(wint_t);
wint_t towupper(wint_t);

#ifdef __cplusplus
}
#endif

#endif // _WCTYPE_H
