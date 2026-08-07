#ifndef _ABIBITS_SOCKADDR_STORAGE_H
#define _ABIBITS_SOCKADDR_STORAGE_H

#include <abi-bits/sa_family_t.h>

struct sockaddr_storage {
	sa_family_t ss_family;
	char __padding[128 - sizeof(sa_family_t) - sizeof(long)];
	long __force_alignment;
};

#endif /* _ABIBITS_SOCKADDR_STORAGE_H */
