
#ifndef _SYS_RANDOM_H
#define _SYS_RANDOM_H

#ifdef __cplusplus
extern "C" {
#endif

#define GRND_RANDOM 1
#define GRND_NONBLOCK 2

#include <bits/ssize_t.h>
#include <bits/size_t.h>

ssize_t getrandom(void *, size_t, unsigned int);

#ifdef __cplusplus
}
#endif

#endif //_SYS_RANDOM_H

