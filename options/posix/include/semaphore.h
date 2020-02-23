#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>

#define SEM_VALUE_MAX 0x7FFFFFFF

typedef struct sem_ {
	unsigned int __mlibc_count;
} sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int initial_count);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abstime);
int sem_post(sem_t *sem);

#ifdef __cplusplus
}
#endif

#endif //_SEMAPHORE_H
