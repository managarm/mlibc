/* $NetBSD: complex.h,v 1.3 2010/09/15 16:11:30 christos Exp $ */

/*
 * Written by Matthias Drochner.
 * Public domain.
 */

#ifndef	_COMPLEX_H
#define	_COMPLEX_H

#define complex _Complex
#define _Complex_I 1.0fi
#define I _Complex_I

#define CMPLX(x, y) ((double complex)__builtin_complex((double)(x), (double)(y)))
#define CMPLXF(x, y) ((float complex)__builtin_complex((float)(x), (float)(y)))
#define CMPLXL(x, y) ((long double complex)__builtin_complex((long double)(x), (long double)(y)))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

/* 7.3.5 Trigonometric functions */
/* 7.3.5.1 The cacos functions */
double complex cacos(double complex __x);
float complex cacosf(float complex __x);

/* 7.3.5.2 The casin functions */
double complex casin(double complex __x);
float complex casinf(float complex __x);

/* 7.3.5.1 The catan functions */
double complex catan(double complex __x);
float complex catanf(float complex __x);

/* 7.3.5.1 The ccos functions */
double complex ccos(double complex __x);
float complex ccosf(float complex __x);

/* 7.3.5.1 The csin functions */
double complex csin(double complex __x);
float complex csinf(float complex __x);

/* 7.3.5.1 The ctan functions */
double complex ctan(double complex __x);
float complex ctanf(float complex __x);

/* 7.3.6 Hyperbolic functions */
/* 7.3.6.1 The cacosh functions */
double complex cacosh(double complex __x);
float complex cacoshf(float complex __x);

/* 7.3.6.2 The casinh functions */
double complex casinh(double complex __x);
float complex casinhf(float complex __x);

/* 7.3.6.3 The catanh functions */
double complex catanh(double complex __x);
float complex catanhf(float complex __x);

/* 7.3.6.4 The ccosh functions */
double complex ccosh(double complex __x);
float complex ccoshf(float complex __x);

/* 7.3.6.5 The csinh functions */
double complex csinh(double complex __x);
float complex csinhf(float complex __x);

/* 7.3.6.6 The ctanh functions */
double complex ctanh(double complex __x);
float complex ctanhf(float complex __x);

/* 7.3.7 Exponential and logarithmic functions */
/* 7.3.7.1 The cexp functions */
double complex cexp(double complex __x);
float complex cexpf(float complex __x);

/* 7.3.7.2 The clog functions */
double complex clog(double complex __x);
float complex clogf(float complex __x);

/* 7.3.8 Power and absolute-value functions */
/* 7.3.8.1 The cabs functions */
/*#ifndef __LIBM0_SOURCE__ */
/* avoid conflict with historical cabs(struct complex) */
/* double cabs(double complex __x) __RENAME(__c99_cabs);
   float cabsf(float complex __x) __RENAME(__c99_cabsf);
   #endif
*/
double cabs(double complex __x);
float cabsf(float complex __x);

/* 7.3.8.2 The cpow functions */
double complex cpow(double complex __x, double complex __y);
float complex cpowf(float complex __x, float complex __y);

/* 7.3.8.3 The csqrt functions */
double complex csqrt(double complex __x);
float complex csqrtf(float complex __x);

/* 7.3.9 Manipulation functions */
/* 7.3.9.1 The carg functions */ 
double carg(double complex __x);
float cargf(float complex __x);

/* 7.3.9.2 The cimag functions */
double cimag(double complex __x);
float cimagf(float complex __x);
long double cimagl(long double complex __x);

/* 7.3.9.3 The conj functions */
double complex conj(double complex __x);
float complex conjf(float complex __x);
/*long double complex conjl(long double complex __x); */

/* 7.3.9.4 The cproj functions */
double complex cproj(double complex __x);
float complex cprojf(float complex __x);
/*long double complex cprojl(long double complex __x); */

/* 7.3.9.5 The creal functions */
double creal(double complex __x);
float crealf(float complex __x);
long double creall(long double complex __x);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif	/* ! _COMPLEX_H */
