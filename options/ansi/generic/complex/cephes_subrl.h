/* $NetBSD: cephes_subrl.h,v 1.1 2014/10/10 00:48:18 christos Exp $ */

__attribute__((__visibility__("hidden")))
void _cchshl(long double, long double *, long double *);
__attribute__((__visibility__("hidden")))
long double _redupil(long double);
__attribute__((__visibility__("hidden")))
long double _ctansl(long double complex);

#define	M_PIL	3.14159265358979323846264338327950280e+00L
#define	M_PI_2L	1.57079632679489661923132169163975140e+00L
