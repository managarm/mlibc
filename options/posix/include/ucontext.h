#ifndef _UCONTEXT_H
#define _UCONTEXT_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef __MLIBC_ABI_ONLY

int getcontext(ucontext_t *);
int setcontext(const ucontext_t *);
void makecontext(ucontext_t *, void (*)(void), int, ...);
int swapcontext(ucontext_t *, const ucontext_t *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _UCONTEXT_H
