#ifndef _PTHREAD_NP_H_
#define _PTHREAD_NP_H_

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int pthread_attr_get_np(pthread_t __thrd, pthread_attr_t *__attr);

#endif /* __MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _PTHREAD_NP_H_ */
