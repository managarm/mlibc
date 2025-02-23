#ifndef _SYS_SCHED2_H
#define _SYS_SCHED2_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_RT     (1 << 0)
#define THREAD_MONO   (1 << 1)
#define THREAD_MLOCK  (1 << 2)
#define THREAD_BANNED (1 << 3)
int get_thread_sched(void);
int set_thread_sched(int flags);
int set_deadlines(int runtime, int period);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SCHED2_H */
