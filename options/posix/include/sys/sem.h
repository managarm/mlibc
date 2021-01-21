#ifndef _SYS_SEM_H
#define _SYS_SEM_H

#include <sys/ipc.h>

#ifdef __cplusplus
extern "C" {
#endif

int semget(key_t, int, int);

#ifdef __cplusplus
}
#endif

#endif // _SYS_SEM_H
