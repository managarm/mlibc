
#ifndef _LINUX_SCHED_H
#define _LINUX_SCHED_H

// Glibc extension
int sched_getcpu(void);
int setns(int fd, int nstype);

#endif // _LINUX_SCHED_H
