#ifndef _MQUEUE_H
#define _MQUEUE_H

#include <abi-bits/mqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int mqd_t;

#ifndef __MLIBC_ABI_ONLY

int mq_getattr(mqd_t __mqdes, struct mq_attr *__attr);
int mq_setattr(mqd_t __mqdes, const struct mq_attr *__restrict__ __newattr, struct mq_attr *__restrict__ __oldattr);
int mq_unlink(const char *__name);
mqd_t mq_open(const char *__name, int __flags, ...);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MQUEUE_H */

