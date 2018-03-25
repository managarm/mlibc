
#ifndef _FENV_H
#define _FENV_H

#include <bits/types.h>

#define FE_DIVBYZERO 4
#define FE_INEXACT 32
#define FE_INVALID 1
#define FE_OVERFLOW 8
#define FE_UNDERFLOW 16
#define __FE_DENORM 2

#define FE_ALL_EXCEPT 63

#define FE_DOWNWARD 0x400
#define FE_TONEAREST 0
#define FE_TOWARDZERO 0xC00
#define FE_UPWARD 0x800

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	__mlibc_uint32 __control_word;
	__mlibc_uint32 __status_word;
	__mlibc_uint32 __unused[5];
	__mlibc_uint32 __mxcsr;
} fenv_t;

typedef __mlibc_uint16 fexcept_t;

int feclearexcept(int);
int fegetenv(fenv_t *);
int fegetexceptflag(fexcept_t *, int);
int fegetround(void);
int feholdexcept(fenv_t *);
int feraiseexcept(int);
int fesetenv(const fenv_t *);
int fesetexceptflag(const fexcept_t *, int);
int fesetround(int);
int fetestexcept(int);
int feupdateenv(const fenv_t *);

#ifdef __cplusplus
}
#endif

#define FE_DFL_ENV ((const fenv_t *) -1)

#endif // _FENV_H

