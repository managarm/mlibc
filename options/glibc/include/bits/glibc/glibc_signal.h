#ifndef MLIBC_GLIBC_SIGNAL_H
#define MLIBC_GLIBC_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int tgkill(int, int, int);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // MLIBC_GLIBC_SIGNAL_H
