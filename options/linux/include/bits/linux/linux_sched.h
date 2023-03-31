
#ifndef _LINUX_SCHED_H
#define _LINUX_SCHED_H

/* Glibc extension */
int sched_getcpu(void);

#if defined(_GNU_SOURCE)
int setns(int fd, int nstype);
#endif /* _GNU_SOURCE */

#endif /* _LINUX_SCHED_H */
