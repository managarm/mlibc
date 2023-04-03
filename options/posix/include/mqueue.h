#ifndef _MQUEUE_H
#define _MQUEUE_H

#include <abi-bits/mqueue.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int mqd_t;

int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
int mq_setattr(mqd_t mqdes, const struct mq_attr *__restrict__ newattr, struct mq_attr *__restrict__ oldattr);
int mq_unlink(const char *name);
mqd_t mq_open(const char *name, int flags, ...);

#ifdef __cplusplus
}
#endif

#endif /* _MQUEUE_H */

