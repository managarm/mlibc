
#ifndef _SYS_UN_H
#define _SYS_UN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/posix/sa_family_t.h>

struct sockaddr_un {
	sa_family_t sun_family;
	char sun_path[108];
};

#ifdef __cplusplus
}
#endif

#endif // _SYS_UN_H

