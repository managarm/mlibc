
#ifndef _SYS_UN_H
#define _SYS_UN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/socket.h>

struct sockaddr_un {
	sa_family_t sun_family;
	char sun_path[108];
};

/* Evaluate to actual length of the `sockaddr_un' structure. */
#define SUN_LEN(ptr) ((size_t) offsetof(struct sockaddr_un, sun_path) + strlen((ptr)->sun_path))

#ifdef __cplusplus
}
#endif

#endif /* _SYS_UN_H */

