#ifndef _SYS_SCHED2_H
#define _SYS_SCHED2_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_RT     0b0001
#define THREAD_MONO   0b0010
#define THREAD_MLOCK  0b0100
#define THREAD_BANNED 0b1000
int get_thread_sched(void);
int set_thread_sched(int flags);
int set_deadlines(int runtime, int period);

#ifdef __cplusplus
}
#endif

#endif // _SYS_SCHED2_H
