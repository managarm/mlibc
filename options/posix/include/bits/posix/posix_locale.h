#ifndef MLIBC_POSIX_LOCALE_H
#define MLIBC_POSIX_LOCALE_H

#include <bits/posix/locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

locale_t newlocale(int category_mask, const char *locale, locale_t base);
void freelocale(locale_t locobj);
locale_t uselocale(locale_t locobj);
locale_t duplocale(locale_t locobj);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_LOCALE_H
