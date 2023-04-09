#ifndef _NETINET_ETHER_H
#define _NETINET_ETHER_H

#include <bits/ether_addr.h>
#include <netinet/if_ether.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

char *ether_ntoa_r(const struct ether_addr *p_a, char *x);

struct ether_addr *ether_aton(const char *asc);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif //_NETINET_ETHER_H
