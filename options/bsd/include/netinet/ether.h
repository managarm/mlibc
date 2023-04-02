#ifndef _NETINET_ETHER_H
#define _NETINET_ETHER_H

#include <netinet/if_ether.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ether_addr {
	uint8_t ether_addr_octet[6];
} __attribute__ ((__packed__));

char *ether_ntoa_r(const struct ether_addr *p_a, char *x);

struct ether_addr *ether_aton(const char *asc);

#ifdef __cplusplus
}
#endif

#endif //_NETINET_ETHER_H
