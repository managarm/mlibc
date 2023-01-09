#ifndef _NETINET_ETHER_H
#define _NETINET_ETHER_H

#include <netinet/if_ether.h>

#ifdef __cplusplus
extern "C" {
#endif

char *ether_ntoa_r(const struct ether_addr *p_a, char *x);

#ifdef __cplusplus
}
#endif

#endif //_NETINET_ETHER_H
