#ifndef _UCONTEXT_H
#define _UCONTEXT_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int getcontext(ucontext_t *);
int setcontext(const ucontext_t *);
void makecontext(ucontext_t *, void (*)(void), int, ...);
int swapcontext(ucontext_t *, const ucontext_t *);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _UCONTEXT_H
