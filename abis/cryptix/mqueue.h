#ifndef _ABIBITS_MQUEUE_H
#define _ABIBITS_MQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

struct mq_attr {
	long mq_flags;
	long mq_maxmsg;
	long mq_msgsize;
	long mq_curmsgs;
	long __pad[4];
};

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_MQUEUE_H */

