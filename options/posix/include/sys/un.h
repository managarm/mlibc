
#ifndef _SYS_UN_H
#define _SYS_UN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mlibc-config.h>

#include <abi-bits/sockaddr_un.h>

#if defined(_DEFAULT_SOURCE)
/* Evaluate to actual length of the `sockaddr_un' structure. */
#define SUN_LEN(ptr) ((size_t) __builtin_offsetof(struct sockaddr_un, sun_path) + strlen((ptr)->sun_path))
#endif /* defined(_DEFAULT_SOURCE) */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_UN_H */

