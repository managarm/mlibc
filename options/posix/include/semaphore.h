#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>

#define SEM_VALUE_MAX 0x7FFFFFFF
#define SEM_FAILED ((sem_t *) 0)

#if defined __x86_64__ && !defined __ILP32__
#define __WORDSIZE 64
#else
#define __WORDSIZE 32
#endif

#if __WORDSIZE == 64
#define __SIZEOF_SEM_T 32
#else
#define __SIZEOF_SEM_T 16
#endif

typedef union {
  char __size[__SIZEOF_SEM_T];
  long int __align;
} sem_t;

#ifndef __MLIBC_ABI_ONLY

int sem_init(sem_t *sem, int pshared, unsigned int initial_count);
sem_t *sem_open(const char *, int, ...);
int sem_close(sem_t *sem);
int sem_unlink(const char *);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abstime);
int sem_post(sem_t *sem);
int sem_getvalue(sem_t *sem, int *sval);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif //_SEMAPHORE_H
