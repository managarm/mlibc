#include <ctype.h>
#include <wctype.h>

#include <bits/ensure.h>

int isalnum_l(int c, locale_t) {
    return isalnum(c);
}

int isalpha_l(int c, locale_t) {
    return isalpha(c);
}

int isblank_l(int c, locale_t) {
    return isblank(c);
}

int iscntrl_l(int c, locale_t) {
    return iscntrl(c);
}

int isdigit_l(int c, locale_t) {
    return isdigit(c);
}

int isgraph_l(int c, locale_t) {
    return isgraph(c);
}

int islower_l(int c, locale_t) {
    return islower(c);
}

int isprint_l(int c, locale_t) {
    return isprint(c);
}

int ispunct_l(int c, locale_t) {
    return ispunct(c);
}

int isspace_l(int c, locale_t) {
    return isspace(c);
}

int isupper_l(int c, locale_t) {
    return isupper(c);
}

int isxdigit_l(int c, locale_t) {
    return isxdigit(c);
}

int isascii_l(int c, locale_t) {
    return isascii(c);
}

int tolower_l(int c, locale_t) {
    return tolower(c);
}

int toupper_l(int c, locale_t) {
    return toupper(c);
}

int iswalnum_l(wint_t c, locale_t) {
    return iswalnum(c);
}

int iswblank_l(wint_t c, locale_t) {
    return iswblank(c);
}

int iswcntrl_l(wint_t c, locale_t) {
    return iswcntrl(c);
}

int iswdigit_l(wint_t c, locale_t) {
    return iswdigit(c);
}

int iswgraph_l(wint_t c, locale_t) {
    return iswgraph(c);
}

int iswlower_l(wint_t c, locale_t) {
    return iswlower(c);
}

int iswprint_l(wint_t c, locale_t) {
    return iswprint(c);
}

int iswpunct_l(wint_t c, locale_t) {
    return iswpunct(c);
}

int iswspace_l(wint_t c, locale_t) {
    return iswspace(c);
}

int iswupper_l(wint_t c, locale_t) {
    return iswupper(c);
}

int iswxdigit_l(wint_t c, locale_t) {
    return iswxdigit(c);
}

int iswalpha_l(wint_t c, locale_t) {
    return iswalpha(c);
}

wctype_t wctype_l(const char* p, locale_t) {
    return wctype(p);
}

int iswctype_l(wint_t w, wctype_t t, locale_t) {
    return iswctype(w, t);
}

wint_t towlower_l(wint_t c, locale_t) {
    return towlower(c);
}

wint_t towupper_l(wint_t c, locale_t) {
    return towupper(c);
}

wctrans_t wctrans_l(const char* c, locale_t) {
    return wctrans(c);
}

wint_t towctrans_l(wint_t c, wctrans_t desc, locale_t) {
    return towctrans(c, desc);
}
