#ifndef _ABIBITS_SOCKADDR_UN_H
#define _ABIBITS_SOCKADDR_UN_H

#include <abi-bits/sa_family_t.h>

struct sockaddr_un {
	sa_family_t sun_family;
	char sun_path[108];
};

#endif /* _ABIBITS_SOCKADDR_UN_H */
