#ifndef _COMPLEX_H
#define _COMPLEX_H

#ifdef __cplusplus
extern "C" {
#endif

// [C11/7.3.1 Introduction]

#define complex _Complex

#define _Complex_I (0.0f + 1.0fi)
#define I _Complex_I

// [C11/7.3.9 Manipulation functions]

double cimag(double complex z);
float cimagf(float complex z);
long double cimagl(long double complex z);

#define CMPLX(x, y) ((double complex)__builtin_complex((double)(x), (double)(y)))
#define CMPLXF(x, y) ((float complex)__builtin_complex((float)(x), (float)(y)))
#define CMPLXL(x, y) ((long double complex)__builtin_complex((long double)(x), (long double)(y)))

double creal(double complex z);
float crealf(float complex z);
long double creall(long double complex z);

#ifdef __cplusplus
}
#endif

#endif // _COMPLEX_H
