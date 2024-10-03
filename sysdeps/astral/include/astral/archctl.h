#ifndef _ASTRAL_ARCHCTL_H
#define _ASTRAL_ARCHCTL_H

#define ARCH_CTL_GSBASE 0
#define ARCH_CTL_FSBASE 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int arch_ctl(int, void *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _ASTRAL_ARCH_CTLH */
