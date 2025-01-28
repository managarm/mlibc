/* $NetBSD: cephes_subr.h,v 1.1 2007/08/20 16:01:33 drochner Exp $ */

#ifndef __MLIBC_ABI_ONLY

void __mlibc_cchsh(double, double *, double *);
double __mlibc_redupi(double);
double __mlibc_ctans(double complex);

#endif /* !__MLIBC_ABI_ONLY */
