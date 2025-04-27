
#ifndef MLIBC_BSD_STDLIB_H
#define MLIBC_BSD_STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int getloadavg(double *__loadavg, int __count);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_BSD_STDLIB_H */

