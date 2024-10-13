
#ifndef _FENV_H
#define _FENV_H

#include <bits/types.h>
#include <bits/ansi/fenv.h>

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

#ifndef __MLIBC_ABI_ONLY

int feclearexcept(int __excepts);
int fegetenv(fenv_t *__envp);
int fegetexceptflag(fexcept_t *__envp, int __excepts);
int fegetround(void);
int feholdexcept(fenv_t *__envp);
int feraiseexcept(int __excepts);
int fesetenv(const fenv_t *__envp);
int fesetexceptflag(const fexcept_t *__envp, int __excepts);
int fesetround(int __round);
int fetestexcept(int __excepts);
int feupdateenv(const fenv_t *__envp);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#define FE_DFL_ENV ((const fenv_t *) -1)

#endif /* _FENV_H */

