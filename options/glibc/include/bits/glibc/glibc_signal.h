#ifndef MLIBC_GLIBC_SIGNAL_H
#define MLIBC_GLIBC_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int tgkill(int __tgid, int __tid, int __sig);

#if defined(_GNU_SOURCE)

typedef void (*sighandler_t)(int __signo);

#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_GLIBC_SIGNAL_H */
