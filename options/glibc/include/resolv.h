#ifndef _RESOLV_H
#define _RESOLV_H

#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

int dn_expand(const unsigned char *, const unsigned char *,
		const unsigned char *, char *, int);

int res_query(const char *, int, int, unsigned char *, int);

#ifdef __cplusplus
}
#endif

#endif // _RESOLV_H
