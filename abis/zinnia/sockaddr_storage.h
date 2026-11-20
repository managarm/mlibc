#ifndef _ABIBITS_SOCKADDR_STORAGE_H
#define _ABIBITS_SOCKADDR_STORAGE_H

#include <abi-bits/sa_family_t.h>
#include <bits/types.h>

struct sockaddr_storage {
	sa_family_t ss_family;
	char __padding[128 - sizeof(sa_family_t) - sizeof(__mlibc_uintptr)];
	__mlibc_uintptr __force_alignment;
};

#endif /* _ABIBITS_SOCKADDR_STORAGE_H */
