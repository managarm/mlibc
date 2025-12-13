#ifndef _MLIBC_GLIBC_SIGNAL_H
#define _MLIBC_GLIBC_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

#if defined(_GNU_SOURCE)

int tgkill(int __tgid, int __tid, int __sig);

typedef void (*sighandler_t)(int __signo);

#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_GLIBC_SIGNAL_H */
