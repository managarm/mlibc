
#ifndef _FENV_H
#define _FENV_H

#include <bits/ansi/fenv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	__UINT32_TYPE__ __control_word;
	__UINT32_TYPE__ __status_word;
	__UINT32_TYPE__ __unused[5];
	__UINT32_TYPE__ __mxcsr;
} fenv_t;

typedef __UINT16_TYPE__ fexcept_t;

#ifndef __MLIBC_ABI_ONLY

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

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#define FE_DFL_ENV ((const fenv_t *) -1)

#endif // _FENV_H

