
#ifndef _MATH_H
#define _MATH_H

#include <bits/inline-definition.h>

/* this is a posix extension */
#define M_E             2.7182818284590452354
#define M_LOG2E         1.4426950408889634074
#define M_LOG10E        0.43429448190325182765
#define M_LN2           0.69314718055994530942
#define M_LN10          2.30258509299404568402
#define M_PI            3.14159265358979323846
#define M_PI_2          1.57079632679489661923
#define M_PI_4          0.78539816339744830962
#define M_1_PI          0.31830988618379067154
#define M_2_PI          0.63661977236758134308
#define M_2_SQRTPI      1.12837916709551257390
#define M_SQRT2         1.41421356237309504880
#define M_SQRT1_2       0.70710678118654752440
#define M_PIl           3.141592653589793238462643383279502884L

/* The following two definitions are from musl. */
#define FP_ILOGBNAN (-1 - (int)(((unsigned)-1) >> 1))
#define FP_ILOGB0 FP_ILOGBNAN

#ifdef __cplusplus
extern "C" {
#endif

typedef double double_t;
typedef float float_t;

#define HUGE_VAL (__builtin_huge_val())
#define HUGE_VALF (__builtin_huge_valf())
#define HUGE_VALL (__builtin_huge_vall())
#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))

/* [C11/7.12.1 Treatment of error conditions] */

#define MATH_ERRNO 1
#define MATH_ERREXCEPT 2
#define math_errhandling 3

/* [C11/7.12.3 Classification macros] */

/* NOTE: fpclassify always returns exactly one of those constants */
/* However making them bitwise disjoint simplifies isfinite() etc. */
#define FP_INFINITE 1
#define FP_NAN 2
#define FP_NORMAL 4
#define FP_SUBNORMAL 8
#define FP_ZERO 16

#ifndef __MLIBC_ABI_ONLY

int __fpclassify(double __x);
int __fpclassifyf(float __x);
int __fpclassifyl(long double __x);

#define fpclassify(x) \
	(sizeof(x) == sizeof(double) ? __fpclassify(x) : \
	(sizeof(x) == sizeof(float) ? __fpclassifyf(x) : \
	(sizeof(x) == sizeof(long double) ? __fpclassifyl(x) : \
	0)))

#define isfinite(x) (fpclassify(x) & (FP_NORMAL | FP_SUBNORMAL | FP_ZERO))
#define isnan(x) (fpclassify(x) == FP_NAN)
#define isinf(x) (fpclassify(x) == FP_INFINITE)
#define isnormal(x) (fpclassify(x) == FP_NORMAL)

/* FIXME: this is gcc specific */
#define signbit(x) (__builtin_signbit(x))

/* [C11/7.12.14 Comparison macros] */
#define isunordered(x,y) (isnan((x)) ? ((void)(y),1) : isnan((y)))

__MLIBC_INLINE_DEFINITION int __mlibc_isless(double_t __x, double_t __y) { return !isunordered(__x, __y) && __x < __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_islessf(float_t __x, float_t __y) { return !isunordered(__x, __y) && __x < __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_islessl(long double __x, long double __y) { return !isunordered(__x, __y) && __x < __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_islessequal(double_t __x, double_t __y) { return !isunordered(__x, __y) && __x <= __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_islessequalf(float_t __x, float_t __y) { return !isunordered(__x, __y) && __x <= __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_islessequall(long double __x, long double __y) { return !isunordered(__x, __y) && __x <= __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_islessgreater(double_t __x, double_t __y) { return !isunordered(__x, __y) && __x != __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_islessgreaterf(float_t __x, float_t __y) { return !isunordered(__x, __y) && __x != __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_islessgreaterl(long double __x, long double __y) { return !isunordered(__x, __y) && __x != __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_isgreater(double_t __x, double_t __y) { return !isunordered(__x, __y) && __x > __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_isgreaterf(float_t __x, float_t __y) { return !isunordered(__x, __y) && __x > __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_isgreaterl(long double __x, long double __y) { return !isunordered(__x, __y) && __x > __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_isgreaterequal(double_t __x, double_t __y) { return !isunordered(__x, __y) && __x >= __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_isgreaterequalf(float_t __x, float_t __y) { return !isunordered(__x, __y) && __x >= __y; }
__MLIBC_INLINE_DEFINITION int __mlibc_isgreaterequall(long double __x, long double __y) { return !isunordered(__x, __y) && __x >= __y; }

/* TODO: We chould use _Generic here but that does not work in C++ code. */
#define __MLIBC_CHOOSE_COMPARISON(x, y, p) ( \
	sizeof((x)+(y)) == sizeof(float) ? p##f(x, y) : \
	sizeof((x)+(y)) == sizeof(double) ? p(x, y) : \
	p##l(x, y) )

#define isless(x, y) __MLIBC_CHOOSE_COMPARISON(x, y, __mlibc_isless)
#define islessequal(x, y) __MLIBC_CHOOSE_COMPARISON(x, y, __mlibc_islessequal)
#define islessgreater(x, y) __MLIBC_CHOOSE_COMPARISON(x, y, __mlibc_islessgreater)
#define isgreater(x, y) __MLIBC_CHOOSE_COMPARISON(x, y, __mlibc_isgreater)
#define isgreaterequal(x, y) __MLIBC_CHOOSE_COMPARISON(x, y, __mlibc_isgreaterequal)

/* this is a gnu extension */
void sincos(double __x, double *__sin, double *__cos);
void sincosf(float __x, float *__sin, float *__cos);
void sincosl(long double __x, long double *__sin, long double *__cos);

double exp10(double __x);
float exp10f(float __x);
long double exp10l(long double __x);

double pow10(double __x);
float pow10f(float __x);
long double pow10l(long double __x);

/* [C11/7.12.4 Trigonometric functions] */

double acos(double __x);
float acosf(float __x);
long double acosl(long double __x);

double asin(double __x);
float asinf(float __x);
long double asinl(long double __x);

double atan(double __x);
float atanf(float __x);
long double atanl(long double __x);

double atan2(double __x, double __y);
float atan2f(float __x, float __y);
long double atan2l(long double __x, long double __y);

double cos(double __x);
float cosf(float __x);
long double cosl(long double __x);

double sin(double __x);
float sinf(float __x);
long double sinl(long double __x);

double tan(double __x);
float tanf(float __x);
long double tanl(long double __x);

/* [C11/7.12.5 Hyperbolic functions] */

double acosh(double __x);
float acoshf(float __x);
long double acoshl(long double __x);

double asinh(double __x);
float asinhf(float __x);
long double asinhl(long double __x);

double atanh(double __x);
float atanhf(float __x);
long double atanhl(long double __x);

double cosh(double __x);
float coshf(float __x);
long double coshl(long double __x);

double sinh(double __x);
float sinhf(float __x);
long double sinhl(long double __x);

double tanh(double __x);
float tanhf(float __x);
long double tanhl(long double __x);

/* [C11/7.12.6 Exponential and logarithmic functions] */

double exp(double __x);
float expf(float __x);
long double expl(long double __x);

double exp2(double __x);
float exp2f(float __x);
long double exp2l(long double __x);

double expm1(double __x);
float expm1f(float __x);
long double expm1l(long double __x);

double frexp(double __x, int *__power);
float frexpf(float __x, int *__power);
long double frexpl(long double __x, int *__power);

int ilogb(double __x);
int ilogbf(float __x);
int ilogbl(long double __x);

double ldexp(double __x, int __power);
float ldexpf(float __x, int __power);
long double ldexpl(long double __x, int __power);

double log(double __x);
float logf(float __x);
long double logl(long double __x);

double log10(double __x);
float log10f(float __x);
long double log10l(long double __x);

double log1p(double __x);
float log1pf(float __x);
long double log1pl(long double __x);

double log2(double __x);
float log2f(float __x);
long double log2l(long double __x);

double logb(double __x);
float logbf(float __x);
long double logbl(long double __x);

double modf(double __x, double *__integral);
float modff(float __x, float *__integral);
long double modfl(long double __x, long double *__integral);

double scalbn(double __x, int __power);
float scalbnf(float __x, int __power);
long double scalbnl(long double __x, int __power);

double scalbln(double __x, long __power);
float scalblnf(float __x, long __power);
long double scalblnl(long double __x, long __power);

/* [C11/7.12.7 Power and absolute-value functions] */

double cbrt(double __x);
float cbrtf(float __x);
long double cbrtl(long double __x);

double fabs(double __x);
float fabsf(float __x);
long double fabsl(long double __x);

double hypot(double __x, double __y);
float hypotf(float __x, float __y);
long double hypotl(long double __x, long double __y);

double pow(double __x, double __y);
float powf(float __x, float __y);
long double powl(long double __x, long double __y);

double sqrt(double __x);
float sqrtf(float __x);
long double sqrtl(long double __x);

/* [C11/7.12.8 Error and gamma functions] */

double erf(double __x);
float erff(float __x);
long double erfl(long double __x);

double erfc(double __x);
float erfcf(float __x);
long double erfcl(long double __x);

double lgamma(double __x);
float lgammaf(float __x);
long double lgammal(long double __x);

double tgamma(double __x);
float tgammaf(float __x);
long double tgammal(long double __x);

/* [C11/7.12.9 Nearest integer functions] */

double ceil(double __x);
float ceilf(float __x);
long double ceill(long double __x);

double floor(double __x);
float floorf(float __x);
long double floorl(long double __x);

double nearbyint(double __x);
float nearbyintf(float __x);
long double nearbyintl(long double __x);

double rint(double __x);
float rintf(float __x);
long double rintl(long double __x);

long lrint(double __x);
long lrintf(float __x);
long lrintl(long double __x);

long long llrint(double __x);
long long llrintf(float __x);
long long llrintl(long double __x);

double round(double __x);
float roundf(float __x);
long double roundl(long double __x);

long lround(double __x);
long lroundf(float __x);
long lroundl(long double __x);

long long llround(double __x);
long long llroundf(float __x);
long long llroundl(long double __x);

double trunc(double __x);
float truncf(float __x);
long double truncl(long double __x);

/* [C11/7.12.10 Remainder functions] */

double fmod(double __x, double __y);
float fmodf(float __x, float __y);
long double fmodl(long double __x, long double __y);

double remainder(double __x, double __y);
float remainderf(float __x, float __y);
long double remainderl(long double __x, long double __y);

double remquo(double __x, double __y, int *__quotient);
float remquof(float __x, float __y, int *__quotient);
long double remquol(long double __x, long double __y, int *__quotient);

/* [C11/7.12.11 Manipulation functions] */

double copysign(double __x, double __sign);
float copysignf(float __x, float __sign);
long double copysignl(long double __x, long double __sign);

double nan(const char *__tag);
float nanf(const char *__tag);
long double nanl(const char *__tag);

double nextafter(double __x, double __dir);
float nextafterf(float __x, float __dir);
long double nextafterl(long double __x, long double __dir);

double nexttoward(double __x, long double __dir);
float nexttowardf(float __x, long double __dir);
long double nexttowardl(long double __x, long double __dir);

/* [C11/7.12.12 Maximum, minimum and positive difference functions] */

double fdim(double __x, double __y);
float fdimf(float __x, float __y);
long double fdiml(long double __x, long double __y);

double fmax(double __x, double __y);
float fmaxf(float __x, float __y);
long double fmaxl(long double __x, long double __y);

double fmin(double __x, double __y);
float fminf(float __x, float __y);
long double fminl(long double __x, long double __y);

/* [C11/7.12.13 Floating multiply-add] */

double fma(double __x, double __y, double __z);
float fmaf(float __x, float __y, float __z);
long double fmal(long double __x, long double __y, long double __z);

extern int signgam;
#define __signgam signgam

/* BSD floating-point classification functions - obsolete */

int finite(double __x);
int finitef(float __x);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MATH_H */

