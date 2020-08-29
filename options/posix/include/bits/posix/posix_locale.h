#ifndef MLIBC_POSIX_LOCALE_H
#define MLIBC_POSIX_LOCALE_H

#include <bits/posix/locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif

locale_t newlocale(int category_mask, const char *locale, locale_t base);
void freelocale(locale_t locobj);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_LOCALE_H
